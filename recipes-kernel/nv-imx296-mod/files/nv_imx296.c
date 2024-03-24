/*
 * imx296.c - imx296 sensor driver
 *
 * Copyright (c) 2016-2022, NVIDIA CORPORATION.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#define DEBUG 1

#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/module.h>
#include <linux/seq_file.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>

#include <media/tegra_v4l2_camera.h>
#include <media/tegracam_core.h>


// Stolen from nv_imx268.c

#include <../nvidia/drivers/media/platform/tegra/camera/camera_gpio.h>
#include "imx296_mode_tbls.h"

static const struct of_device_id imx296_of_match[] = {
        { .compatible = "sony,imx296", .data = NULL },
        { .compatible = "sony,imx296ll", .data = (void *)IMX296_SENSOR_INFO_IMX296LL },
        { .compatible = "sony,imx296lq", .data = (void *)IMX296_SENSOR_INFO_IMX296LQ },
        { /* sentinel */ },
};

MODULE_DEVICE_TABLE(of, imx296_of_match);

struct imx296_clk_params {
        unsigned int freq;
        u8 incksel[4];
        u8 ctrl418c;
};

static const struct imx296_clk_params imx296_clk_params[] = {
        { 37125000, { 0x80, 0x0b, 0x80, 0x08 }, 116 },
        { 54000000, { 0xb0, 0x0f, 0xb0, 0x0c }, 168 },
        { 74250000, { 0x80, 0x0f, 0x80, 0x0c }, 232 },
};

static const u32 ctrl_cid_list[] = {
	TEGRA_CAMERA_CID_GAIN,
	TEGRA_CAMERA_CID_EXPOSURE,
	TEGRA_CAMERA_CID_FRAME_RATE,
	TEGRA_CAMERA_CID_SENSOR_MODE_ID,
};

struct imx296 {
	struct i2c_client *i2c_client;
	struct v4l2_subdev *subdev;
	u32 vmax;
	struct camera_common_data *s_data;
	struct tegracam_device *tc_dev;

        const struct imx296_clk_params *clk_params;
};

static const struct regmap_config sensor_regmap_config = {
	.reg_bits = 16,
	.val_bits = 8,

	.wr_table = &(const struct regmap_access_table) {
		.no_ranges = (const struct regmap_range[]) {
			{
				.range_min = IMX296_SENSOR_INFO & 0xffff,
				.range_max = (IMX296_SENSOR_INFO & 0xffff) + 1,
			},
		},
		.n_no_ranges = 1,
	},
	//.cache_type = REGCACHE_RBTREE,
//#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 4, 0)
	//.use_single_rw = true,
//#else
	//.use_single_read = true,
	//.use_single_write = true,
//#endif
};

/*
static inline void imx296_get_coarse_time_regs(imx296_reg *regs,
	u32 coarse_time)
{
	regs->addr = IMX296_COARSE_INTEG_TIME_ADDR_MSB;
	regs->val = (coarse_time >> 8) & 0xff;
	(regs + 1)->addr = IMX296_COARSE_INTEG_TIME_ADDR_LSB;
	(regs + 1)->val = (coarse_time) & 0xff;
}

static inline void imx296_get_gain_regs(imx296_reg *regs, s16 gain)
{
	regs->addr = IMX296_GAIN_ADDR_MSB;
	regs->val = (gain >> 8) & 0x01;
	(regs + 1)->addr = IMX296_GAIN_ADDR_LSB;
	(regs + 1)->val = (gain) & 0xff;
}

static inline void imx296_get_frame_length_regs(imx296_reg *regs,
	u32 frame_length)
{
	regs->addr = IMX296_FRAME_LENGTH_ADDR_MSB;
	regs->val = (frame_length >> 8) & 0xff;
	(regs + 1)->addr = IMX296_FRAME_LENGTH_ADDR_LSB;
	(regs + 1)->val = (frame_length) & 0xff;
}
*/

static int imx296_read(struct camera_common_data *s_data, u32 addr)
{
	int result1; //, result2;
	{
		u8 data[3] = { 0, 0, 0 };
		int ret;

		ret = regmap_raw_read(s_data->regmap,
				      addr & IMX296_REG_ADDR_MASK, data,
				      (addr >> IMX296_REG_SIZE_SHIFT) & 3);
		if (ret < 0)
			return ret;

		result1 = (data[2] << 16) | (data[1] << 8) | data[0];
	}

        dev_dbg(s_data->dev, "%s: i2c read, 0x%x = %x",
			__func__, addr, result1);

        /*
	{
		u8 data[3] = { 0, 0, 0 };
		int ret;

		int bytes = (addr >> IMX296_REG_SIZE_SHIFT) & 3;

		if (bytes <= 1) {
                        unsigned int x;
                        ret = regmap_read(s_data->regmap,
                                          addr & IMX296_REG_ADDR_MASK, &x);
                        if (ret < 0)
				return ret;
                        dev_err(s_data->dev, "%s: Read address 0x%x -> 0x%x",
                                __func__, (addr & IMX296_REG_ADDR_MASK), x);
                        data[bytes - 1] = x;
		}
		if (bytes <= 2) {
                        unsigned int x;
			ret = regmap_read(s_data->regmap,
					  (addr - 1) & IMX296_REG_ADDR_MASK,
					  &x);
			if (ret < 0)
				return ret;
                        dev_err(s_data->dev, "%s: Read address 0x%x -> 0x%x",
                                __func__, ((addr - 1) & IMX296_REG_ADDR_MASK), x);
                        data[bytes - 2] = x;
		}
		if (bytes <= 3) {
                        unsigned int x;
			ret = regmap_read(s_data->regmap,
					  (addr - 2) & IMX296_REG_ADDR_MASK,
					  &x);
			if (ret < 0)
				return ret;
                        dev_err(s_data->dev, "%s: Read address 0x%x -> 0x%x",
                                __func__, ((addr - 2) & IMX296_REG_ADDR_MASK), x);
                        data[bytes - 3] = x;
		}

		result2 = (data[2] << 16) | (data[1] << 8) | data[0];
	}
        if (result1 != result2) {
          dev_err(s_data->dev, "%s: Method s do not match, 0x%x != 0x%x", __func__, result1, result2);
        }
        */
        return result1;
}

static int imx296_write(struct camera_common_data *s_data, u32 addr, u32 value,
			int *err)
{
	u8 data[3] = { value & 0xff, (value >> 8) & 0xff, value >> 16 };
	int ret;

        dev_dbg(s_data->dev, "%s: i2c write, 0x%x = %x",
			__func__, addr, value);

	if (err && *err)
		return *err;

	ret = regmap_raw_write(s_data->regmap, addr & IMX296_REG_ADDR_MASK,
			       data, (addr >> IMX296_REG_SIZE_SHIFT) & 3);
	if (ret < 0) {
		dev_err(s_data->dev, "%u-bit write to 0x%04x failed: %d\n",
			((addr >> IMX296_REG_SIZE_SHIFT) & 3) * 8,
			addr & IMX296_REG_ADDR_MASK, ret);
		if (err)
			*err = ret;
	}

	return ret;
}

/*
static inline int imx296_write_table(struct imx296 *priv,
	const imx296_reg table[])
{
	return regmap_util_write_table_8(priv->s_data->regmap, table, NULL, 0,
		IMX296_TABLE_WAIT_MS, IMX296_TABLE_END);
}
*/

// Group hold starts buffering registers until it is stopped so they all go love together.
static int imx296_set_group_hold(struct tegracam_device *tc_dev, bool val)
{
	struct camera_common_data *s_data = tc_dev->s_data;
	struct device *dev = tc_dev->dev;
	int err = 0;

	dev_dbg(dev, "%s: %s group_hold\n",
		__func__, (val ? "enabling" : "disabling"));

	imx296_write(s_data, IMX296_CTRL08, val ? IMX296_CTRL08_REGHOLD : 0, &err);

	if (err) {
		dev_dbg(dev, "%s: group hold control error\n", __func__);
		return err;
	}

	return 0;
}

static int imx296_set_gain(struct tegracam_device *tc_dev, s64 val)
{
	struct camera_common_data *s_data = tc_dev->s_data;
	struct device *dev = tc_dev->dev;
	const struct sensor_mode_properties *mode =
		&(s_data->sensor_props.sensor_modes[s_data->mode_prop_idx]);
	int err = 0;
	s16 gain;
	dev_dbg(dev, "%s: start imx296_set_gain\n", __func__);

	if (val < mode->control_properties.min_gain_val)
		val = mode->control_properties.min_gain_val;
	else if (val > mode->control_properties.max_gain_val)
		val = mode->control_properties.max_gain_val;

	/* translate value (from normalized analog gain) */
	gain = val;

	if (gain < IMX296_GAIN_MIN)
		gain = IMX296_GAIN_MIN;
	else if (gain > IMX296_GAIN_MAX)
		gain = IMX296_GAIN_MAX;

	dev_dbg(dev, "%s: val: %lld [times], gain: %d\n", __func__, val, gain);

	imx296_write(s_data, IMX296_GAIN, gain, &err);

	if (err) {
		dev_dbg(dev, "%s: gain control error\n", __func__);
	}

	return err;
}

static unsigned int
imx296_compute_vmax(const struct sensor_mode_properties *mode, u64 framerate)
{
	/*
	 * Horizontal blanking is controlled through the HMAX register, which
	 * contains a line length in contains a line length in units of an
	 * internal 74.25 MHz clock derived from the INCLK. The HMAX value is
	 * currently fixed to 1100, convert it to a number of pixels based on
	 * the nominal pixel rate.
	 *
	 * Horizontal blanking is fixed, regardless of the crop width, so
	 * ensure the hblank limits are adjusted to account for this.
	 */
	//unsigned int hblank = 1100 * 1188000000ULL / 10 / 74250000 - width;
                           //         pix/sec           / (cycles/sec)

	/*
	 * The sensor calculates the MIPI timings internally to achieve a bit
	 * rate between 1122 and 1198 Mbps. The exact value is unfortunately not
	 * reported, at least according to the documentation. Report a nominal
	 * rate of 1188 Mbps as that is used by the datasheet in multiple
	 * examples.
	 */
        //unsigned int pixel_rate = 1188000000ULL / 10;

        // time_per_line = (width + hblank) / pixel_rate 
        // time_for_capture = height * time_per_line;
        // time_to_wait = (1.0 / framerate - time_for_capture);
        // vblank = lines_to_wait = time_to_wait / time_per_line;
        // vmax = vblank + height;

        // time_per_line = (1100 / 74250000);
        // time_for_capture = height * 1100 / 74250000;
        // vmax =  time_to_wait / time_per_line + height;
        // vmax =  (1.0 / framerate - time_for_capture) / time_per_line + height;
        // vmax =  (1.0 / framerate - height * time_per_line) / time_per_line + height;
        // vmax =  (1.0 / framerate / time_per_line);
        // vmax =  (1.0 / framerate / (1100 / 74250000));

	unsigned int vmax = min_t(
		u64,
		max_t(u64,
		      ((u64)(74250000ull *
			     (u64)mode->control_properties.framerate_factor) /
		       (framerate * 1100ull)),
		      30 + IMX296_PIXEL_ARRAY_HEIGHT),
		1048575);

        return vmax;
}

static int imx296_set_frame_rate(struct tegracam_device *tc_dev, s64 val)
{
	struct camera_common_data *s_data = tc_dev->s_data;
	struct imx296 *priv = (struct imx296 *)tc_dev->priv;
	struct device *dev = tc_dev->dev;
	const struct sensor_mode_properties *mode =
		&(s_data->sensor_props.sensor_modes[s_data->mode_prop_idx]);
	int err = 0;

	unsigned int vmax = imx296_compute_vmax(mode, val);
	dev_dbg(dev,
		"%s: val: %llde-6 [fps], vmax: %u [lines/frame]\n",
		__func__, val, vmax);

	imx296_write(s_data, IMX296_VMAX, vmax, &err);
        if (err == 0) {
		imx296_write(s_data, IMX296_HMAX, 1100, &err);
                priv->vmax = vmax;
        }

	//frame_length = (u32)(mode->signal_properties.pixel_clock.val *
		//(u64)mode->control_properties.framerate_factor /
		//mode->image_properties.line_length / val);

	return err;
}

static int imx296_set_exposure(struct tegracam_device *tc_dev, s64 val)
{
	struct camera_common_data *s_data = tc_dev->s_data;
	struct imx296 *priv = (struct imx296 *)tc_dev->priv;
	struct device *dev = tc_dev->dev;
        const struct sensor_mode_properties *mode =
            &(s_data->sensor_props.sensor_modes[s_data->mode_prop_idx]);

        int err = 0;

        s64 new_val = min_t(s64, val, ((s64)(priv->vmax - 4)));
        u32 shs1 = priv->vmax - new_val;

        dev_dbg(s_data->dev,
                "%s: Setting exposure control to: %u, val is %lld, new_val "
                "%lld vmax %u\n",
                __func__, shs1, val, new_val, priv->vmax);

        // val -> us
        // mode->control_properties.exposure_factor

        imx296_write(s_data, IMX296_SHS1, shs1, &err);

        return err;
}

static struct tegracam_ctrl_ops imx296_ctrl_ops = {
	.numctrls = ARRAY_SIZE(ctrl_cid_list),
	.ctrl_cid_list = ctrl_cid_list,
	.set_gain = imx296_set_gain,
	.set_exposure = imx296_set_exposure,
	.set_frame_rate = imx296_set_frame_rate,
	.set_group_hold = imx296_set_group_hold,
};

static inline int imx296_power_on(struct camera_common_data *s_data)
{
  // Looks reasonable.
	int err = 0;
	struct camera_common_power_rail *pw = s_data->power;
	struct camera_common_pdata *pdata = s_data->pdata;
	struct device *dev = s_data->dev;

	dev_dbg(dev, "%s: power on\n", __func__);
	if (pdata && pdata->power_on) {
		err = pdata->power_on(pw);
		if (err)
			dev_err(dev, "%s: power on failed\n", __func__);
		else
			pw->state = SWITCH_ON;
		return err;
	}

	if (gpio_is_valid(pw->reset_gpio)) {
		if (gpio_cansleep(pw->reset_gpio))
			gpio_set_value_cansleep(pw->reset_gpio, 0);
		else
			gpio_set_value(pw->reset_gpio, 0);
        }

	usleep_range(15, 20);

	if (pw->avdd)
		err = regulator_enable(pw->avdd);
	if (err)
		goto imx296_avdd_fail;

	if (pw->iovdd)
		err = regulator_enable(pw->iovdd);
	if (err)
		goto imx296_iovdd_fail;

	if (pw->dvdd)
		err = regulator_enable(pw->dvdd);
	if (err)
		goto imx296_dvdd_fail;

	usleep_range(5000, 5010);

	if (gpio_is_valid(pw->reset_gpio)) {
		if (gpio_cansleep(pw->reset_gpio))
			gpio_set_value_cansleep(pw->reset_gpio, 1);
		else
			gpio_set_value(pw->reset_gpio, 1);
        }

	usleep_range(300000, 300100);
	//usleep_range(12000, 12010);

	pw->state = SWITCH_ON;

	return 0;

imx296_dvdd_fail:
	regulator_disable(pw->iovdd);

imx296_iovdd_fail:
	regulator_disable(pw->avdd);

imx296_avdd_fail:
	dev_err(dev, "%s: power on failed\n", __func__);

	return err;
}

static int imx296_power_off(struct camera_common_data *s_data)
{
	int err = 0;
	struct camera_common_power_rail *pw = s_data->power;
	struct camera_common_pdata *pdata = s_data->pdata;
	struct device *dev = s_data->dev;

	dev_dbg(dev, "%s: power off\n", __func__);

	if (pdata && pdata->power_off) {
		err = pdata->power_off(pw);
		if (err) {
			dev_err(dev, "%s: power off failed\n", __func__);
			return err;
		}
	} else {
		if (gpio_is_valid(pw->reset_gpio))
			gpio_set_value(pw->reset_gpio, 0);

		usleep_range(15, 20);

		if (pw->avdd)
			regulator_disable(pw->avdd);
		if (pw->iovdd)
			regulator_disable(pw->iovdd);
		if (pw->dvdd)
			regulator_disable(pw->dvdd);
	}

	pw->state = SWITCH_OFF;

	return 0;
}

static int imx296_power_put(struct tegracam_device *tc_dev)
{
	struct camera_common_data *s_data = tc_dev->s_data;
	struct camera_common_power_rail *pw = s_data->power;

	if (unlikely(!pw))
		return -EFAULT;

	if (likely(pw->dvdd))
		devm_regulator_put(pw->dvdd);

	if (likely(pw->avdd))
		devm_regulator_put(pw->avdd);

	if (likely(pw->iovdd))
		devm_regulator_put(pw->iovdd);

	pw->dvdd = NULL;
	pw->avdd = NULL;
	pw->iovdd = NULL;

	if (likely(pw->reset_gpio))
		gpio_free(pw->reset_gpio);

	return 0;
}

static int imx296_power_get(struct tegracam_device *tc_dev)
{
	struct device *dev = tc_dev->dev;
	struct camera_common_data *s_data = tc_dev->s_data;
	struct camera_common_power_rail *pw = s_data->power;
	struct camera_common_pdata *pdata = s_data->pdata;
	const char *mclk_name;
	const char *parentclk_name;
	struct clk *parent;
	int err = 0;

	if (!pdata) {
		dev_err(dev, "pdata missing\n");
		return -EFAULT;
	}

	mclk_name = pdata->mclk_name ? pdata->mclk_name : "cam_mclk1";
	pw->mclk = devm_clk_get(dev, mclk_name);
	if (IS_ERR(pw->mclk)) {
		dev_err(dev, "unable to get clock %s\n", mclk_name);
		return PTR_ERR(pw->mclk);
	}

	parentclk_name = pdata->parentclk_name;
	if (parentclk_name) {
		parent = devm_clk_get(dev, parentclk_name);
		if (IS_ERR(parent)) {
			dev_err(dev, "unable to get parent clock %s",
				parentclk_name);
		} else {
			clk_set_parent(pw->mclk, parent);
		}
	}

	/* analog 2.8v */
	if (pdata->regulators.avdd)
	        err |= camera_common_regulator_get(dev,
		                                        &pw->avdd, pdata->regulators.avdd);
	/* IO 1.8v */
	if (pdata->regulators.iovdd)
	        err |= camera_common_regulator_get(dev,
		        &pw->iovdd, pdata->regulators.iovdd);
	/* dig 1.2v */
	if (pdata->regulators.dvdd)
	        err |= camera_common_regulator_get(dev,
		        &pw->dvdd, pdata->regulators.dvdd);

	if (err) {
		dev_err(dev, "%s: unable to get regulator(s)\n", __func__);
		goto done;
	}

	/* Reset or ENABLE GPIO */
	pw->reset_gpio = pdata->reset_gpio;
	err = gpio_request(pw->reset_gpio, "cam_reset_gpio");
	if (err < 0) {
		dev_err(dev, "%s: unable to request reset_gpio (%d)\n",
			__func__, err);
		goto done;
	}

done:
	pw->state = SWITCH_OFF;

	return err;
}

static struct camera_common_pdata *imx296_parse_dt(struct tegracam_device *tc_dev)
{
	struct device *dev = tc_dev->dev;
	struct device_node *np = dev->of_node;
	struct camera_common_pdata *board_priv_pdata;
	const struct of_device_id *match;
	int err = 0;
	struct camera_common_pdata *ret = NULL;
	int gpio;

	if (!np)
		return NULL;

	match = of_match_device(imx296_of_match, dev);
	if (!match) {
		dev_err(dev, "Failed to find matching dt id\n");
		return NULL;
	}

	board_priv_pdata = devm_kzalloc(dev,
		sizeof(*board_priv_pdata), GFP_KERNEL);
	if (!board_priv_pdata)
		return NULL;

	err = of_property_read_string(np, "mclk", &board_priv_pdata->mclk_name);
	if (err)
		dev_err(dev, "mclk not in DT\n");

	gpio = of_get_named_gpio(np, "reset-gpios", 0);
	if (gpio < 0) {
		if (gpio == -EPROBE_DEFER)
			ret = ERR_PTR(-EPROBE_DEFER);
		dev_err(dev, "reset-gpios not found %d\n", err);
		goto error;
	}
	board_priv_pdata->reset_gpio = (unsigned int)gpio;

	err = of_property_read_string(np, "avdd-reg",
				      &board_priv_pdata->regulators.avdd);
	err |= of_property_read_string(np, "iovdd-reg",
				       &board_priv_pdata->regulators.iovdd);
	err |= of_property_read_string(np, "dvdd-reg",
				       &board_priv_pdata->regulators.dvdd);
	if (err)
		dev_dbg(dev, "avdd, iovdd and/or dvdd reglrs. not present, "
			"assume sensor powered independently\n");

	board_priv_pdata->has_eeprom =
		of_property_read_bool(np, "has-eeprom");

	return board_priv_pdata;

error:
	devm_kfree(dev, board_priv_pdata);

	return ret;
}

static int imx296_set_mode(struct tegracam_device *tc_dev)
{
	struct imx296 *priv = (struct imx296 *)tegracam_get_privdata(tc_dev);
	struct camera_common_data *s_data = tc_dev->s_data;
	int err;
        int i;

	dev_dbg(tc_dev->dev, "%s: Mode is %d\n", __func__, s_data->mode);

	if (s_data->mode != 0) {
	        dev_err(tc_dev->dev, "%s: Mode %d != 0\n", __func__, s_data->mode);
		return -EINVAL;
        }

	dev_dbg(tc_dev->dev, "%s: Writing init table\n", __func__);
	for (i = 0; i < ARRAY_SIZE(imx296_init_table); ++i) {
		imx296_write(s_data, imx296_init_table[i].reg,
			     imx296_init_table[i].value, &err);
        }

	dev_dbg(tc_dev->dev, "%s: Writing roi, err: %d\n", __func__, err);
	imx296_write(s_data, IMX296_FID0_ROI, 0, &err);
	dev_dbg(tc_dev->dev, "%s: Writing height, err: %d\n", __func__, err);
	imx296_write(s_data, IMX296_MIPIC_AREA3W, IMX296_PIXEL_ARRAY_HEIGHT,
		     &err);

	dev_dbg(tc_dev->dev, "%s: Writing ctrl0d, err: %d\n", __func__, err);
	imx296_write(s_data, IMX296_CTRL0D, 0, &err);

	/*
	 * HMAX and VMAX configure horizontal and vertical blanking by
	 * specifying the total line time and frame time respectively. The line
	 * time is specified in operational clock units (which appears to be the
	 * output of an internal PLL, fixed at 74.25 MHz regardless of the
	 * exernal clock frequency), while the frame time is specified as a
	 * number of lines.
	 *
	 * In the vertical direction the sensor outputs the following:
	 *
	 * - one line for the FS packet
	 * - two lines of embedded data (DT 0x12)
	 * - six null lines (DT 0x10)
	 * - four lines of vertical effective optical black (DT 0x37)
	 * - 8 to 1088 lines of active image data (RAW10, DT 0x2b)
	 * - one line for the FE packet
	 * - 16 or more lines of vertical blanking
	 */

	dev_dbg(tc_dev->dev, "%s: Writing hmax, err: %d\n", __func__, err);
	imx296_write(s_data, IMX296_HMAX, 1100, &err);
	dev_dbg(tc_dev->dev, "%s: Writing vmax, err: %d\n", __func__, err);
	imx296_write(s_data, IMX296_VMAX, priv->vmax, &err);

        dev_dbg(tc_dev->dev, "%s: Writing clock params for clock %d, err: %d\n",
                __func__, priv->clk_params->freq, err);
        for (i = 0; i < ARRAY_SIZE(priv->clk_params->incksel); ++i)
		imx296_write(s_data, IMX296_INCKSEL(i),
			     priv->clk_params->incksel[i], &err);
        dev_dbg(tc_dev->dev, "%s: Writing gttablenum, err: %d\n", __func__, err);
	imx296_write(s_data, IMX296_GTTABLENUM, 0xc5, &err);
        dev_dbg(tc_dev->dev, "%s: Writing ctrl418c, err: %d\n", __func__, err);
	imx296_write(s_data, IMX296_CTRL418C, priv->clk_params->ctrl418c,
		     &err);

        dev_dbg(tc_dev->dev, "%s: Writing gaindelay, err: %d\n", __func__, err);
	imx296_write(s_data, IMX296_GAINDLY, IMX296_GAINDLY_1FRAME, &err);
        dev_dbg(tc_dev->dev, "%s: Writing blklevel, err: %d\n", __func__, err);
	imx296_write(s_data, IMX296_BLKLEVEL, 0x03c, &err);

        dev_dbg(tc_dev->dev, "%s: Done, err: %d\n", __func__, err);
	return err;
}

static int imx296_start_streaming(struct tegracam_device *tc_dev)
{
	struct camera_common_data *s_data = tc_dev->s_data;
	int ret = 0;

	imx296_write(s_data, IMX296_CTRL00, 0, &ret);
	usleep_range(2000, 5000);

	imx296_write(s_data, IMX296_CTRL0A, 0, &ret);

	return ret;
}

static int imx296_stop_streaming(struct tegracam_device *tc_dev)
{
	struct camera_common_data *s_data = tc_dev->s_data;
	int ret = 0;

	imx296_write(s_data, IMX296_CTRL0A, IMX296_CTRL0A_XMSTA, &ret);
	imx296_write(s_data, IMX296_CTRL00, IMX296_CTRL00_STANDBY, &ret);

        return ret;
}

static inline int imx296_write_reg(struct camera_common_data *s_data,
				   u16 addr, u8 val)
{
        dev_dbg(s_data->dev, "%s: failing i2c write, 0x%x = %x",
			__func__, addr, val);
  return -EINVAL;
}

static inline int imx296_read_reg(struct camera_common_data *s_data,
				  u16 addr, u8 * val)
{
        dev_dbg(s_data->dev, "%s: failing i2c read, 0x%x = %x",
			__func__, addr, val);
        return -EINVAL;
}

static struct camera_common_sensor_ops imx296_common_ops = {
	.numfrmfmts = ARRAY_SIZE(imx296_frmfmt),
	.frmfmt_table = imx296_frmfmt,
	.power_on = imx296_power_on,
	.power_off = imx296_power_off,
	.write_reg = imx296_write_reg,
	.read_reg = imx296_read_reg,
	.parse_dt = imx296_parse_dt,
	.power_get = imx296_power_get,
	.power_put = imx296_power_put,
	.set_mode = imx296_set_mode,
	.start_streaming = imx296_start_streaming,
	.stop_streaming = imx296_stop_streaming,
};

static int imx296_read_temperature(struct imx296 *sensor, int *temp)
{
	int tmdout;
	int ret;

	ret = imx296_write(sensor->s_data, IMX296_TMDCTRL, IMX296_TMDCTRL_LATCH, NULL);
	if (ret < 0)
		return ret;

	tmdout = imx296_read(sensor->s_data, IMX296_TMDOUT) & IMX296_TMDOUT_MASK;
	if (tmdout < 0)
		return tmdout;

	/* T(°C) = 246.312 - 0.304 * TMDOUT */;
	*temp = 246312 - 304 * tmdout;

	return imx296_write(sensor->s_data, IMX296_TMDCTRL, 0, NULL);
}

static int imx296_identify_model(struct imx296 *sensor)
{
	unsigned int model;
	int temp = 0;
	int ret;

	model = (uintptr_t)of_device_get_match_data(sensor->tc_dev->dev);
	if (model) {
		dev_dbg(sensor->tc_dev->dev,
			"sensor model auto-detection disabled, forcing 0x%04x\n",
			model);
		dev_err(sensor->tc_dev->dev, "Mono not supported\n");
		ret = -ENODEV;
		goto done;
	}

	/*
	 * While most registers can be read when the sensor is in standby, this
	 * is not the case of the sensor info register :-(
	 */
	ret = imx296_write(sensor->s_data, IMX296_CTRL00, 0, NULL);
	if (ret < 0) {
		dev_err(sensor->tc_dev->dev,
			"failed to get sensor out of standby (%d)\n", ret);
		return ret;
	}

	usleep_range(2000, 5000);

	ret = imx296_read(sensor->s_data, IMX296_SENSOR_INFO);
	if (ret < 0) {
		dev_err(sensor->tc_dev->dev, "failed to read sensor information (%d)\n",
			ret);
		goto done;
	}

	model = (ret >> 6) & 0x1ff;

	switch (model) {
	case 296:
                if (ret & IMX296_SENSOR_INFO_MONO) {
		        dev_err(sensor->tc_dev->dev, "Mono not supported\n");
		        ret = -ENODEV;
		        goto done;
                }
		break;
	/*
	 * The IMX297 seems to share features with the IMX296, it may be
	 * possible to support it in the same driver.
	 */
	case 297:
	default:
		dev_err(sensor->tc_dev->dev, "invalid device model 0x%04x\n", ret);
		ret = -ENODEV;
		goto done;
	}

	ret = imx296_read_temperature(sensor, &temp);
	if (ret < 0)
		goto done;

	dev_info(sensor->tc_dev->dev, "found IMX%uLQ (%u.%uC)\n", model,
		 temp / 1000, (temp / 100) % 10);

done:
	imx296_write(sensor->s_data, IMX296_CTRL00, IMX296_CTRL00_STANDBY, NULL);
	return ret;
}

static int imx296_board_setup(struct imx296 *priv)
{
	int err = 0;
	struct camera_common_data *s_data = priv->s_data;
	struct device *dev = s_data->dev;

	dev_dbg(dev, "%s++\n", __func__);

	err = camera_common_mclk_enable(s_data);
	if (err) {
		dev_err(dev, "Error %d turning on mclk\n", err);
		return err;
	}

	err = imx296_power_on(s_data);
	if (err) {
		dev_err(dev, "Error %d during power on sensor\n", err);
		return err;
	}

	err = imx296_identify_model(priv);

	imx296_power_off(s_data);
	camera_common_mclk_disable(s_data);

	return err;
}

static int imx296_open(struct v4l2_subdev *sd, struct v4l2_subdev_fh *fh)
{
	struct i2c_client *client = v4l2_get_subdevdata(sd);

	dev_dbg(&client->dev, "%s:\n", __func__);

	return 0;
}

static const struct v4l2_subdev_internal_ops imx296_subdev_internal_ops = {
	.open = imx296_open,
};

static int imx296_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
{
	struct device *dev = &client->dev;
	struct tegracam_device *tc_dev;
	struct imx296 *priv;
        unsigned long clk_rate;
        int i;
	int err;

	dev_dbg(dev, "[imx296]: probing v4l2 sensor at addr 0x%0x.\n",
		client->addr);

	if (!IS_ENABLED(CONFIG_OF) || !client->dev.of_node)
		return -EINVAL;

	priv = devm_kzalloc(dev, sizeof(struct imx296), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	tc_dev = devm_kzalloc(dev, sizeof(struct tegracam_device), GFP_KERNEL);
	if (!tc_dev)
		return -ENOMEM;

	priv->i2c_client = tc_dev->client = client;
	tc_dev->dev = dev;
	strncpy(tc_dev->name, "imx296", sizeof(tc_dev->name));
	tc_dev->dev_regmap_config = &sensor_regmap_config;
	tc_dev->sensor_ops = &imx296_common_ops;
	tc_dev->v4l2sd_internal_ops = &imx296_subdev_internal_ops;
	tc_dev->tcctrl_ops = &imx296_ctrl_ops;

	err = tegracam_device_register(tc_dev);
	if (err) {
		dev_err(dev, "tegra camera driver registration failed\n");
		return err;
	}
	priv->tc_dev = tc_dev;
	priv->s_data = tc_dev->s_data;
	priv->subdev = &tc_dev->s_data->subdev;
	tegracam_set_privdata(tc_dev, (void *)priv);

        clk_rate = clk_get_rate(priv->s_data->power->mclk);
        for (i = 0; i < ARRAY_SIZE(imx296_clk_params); ++i) {
                if (clk_rate == imx296_clk_params[i].freq) {
                        priv->clk_params = &imx296_clk_params[i];
                        break;
                }
        }

        if (!priv->clk_params) {
                dev_err(dev, "unsupported clock rate %lu\n", clk_rate);
                return -EINVAL;
        }


	err = imx296_board_setup(priv);
	if (err) {
		tegracam_device_unregister(tc_dev);
		dev_err(dev, "board setup failed\n");
		return err;
	}

	err = tegracam_v4l2subdev_register(tc_dev, true);
	if (err) {
		dev_err(dev, "tegra camera subdev registration failed\n");
		return err;
	}

	dev_dbg(dev, "detected imx296 sensor\n");

        const struct sensor_mode_properties *mode =
            &(priv->s_data->sensor_props
                  .sensor_modes[priv->s_data->mode_prop_idx]);
        priv->vmax =
            imx296_compute_vmax(mode, mode->control_properties.max_framerate);

        return 0;
}

static int imx296_remove(struct i2c_client *client)
{
	struct camera_common_data *s_data = to_camera_common_data(&client->dev);
	struct imx296 *priv = (struct imx296 *)s_data->priv;

	tegracam_v4l2subdev_unregister(priv->tc_dev);
	tegracam_device_unregister(priv->tc_dev);

	return 0;
}

static const struct i2c_device_id imx296_id[] = {
	{ "imx296", 0 },
	{ }
};

static struct i2c_driver imx296_i2c_driver = {
	.driver = {
		.of_match_table = of_match_ptr(imx296_of_match),
		.name = "imx296",
		.owner = THIS_MODULE,
	},
	.probe = imx296_probe,
	.remove = imx296_remove,
	.id_table = imx296_id,
};

module_i2c_driver(imx296_i2c_driver);

MODULE_DESCRIPTION("Media Controller driver for Sony IMX296");
MODULE_AUTHOR("NVIDIA Corporation");
MODULE_LICENSE("GPL v2");
