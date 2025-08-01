/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ACPI_ACPI_PLD_H__
#define __ACPI_ACPI_PLD_H__

enum acpi_pld_panel {
	PLD_PANEL_TOP,
	PLD_PANEL_BOTTOM,
	PLD_PANEL_LEFT,
	PLD_PANEL_RIGHT,
	PLD_PANEL_FRONT,
	PLD_PANEL_BACK,
	PLD_PANEL_UNKNOWN
};

enum acpi_pld_vertical_position {
	PLD_VERTICAL_POSITION_UPPER,
	PLD_VERTICAL_POSITION_CENTER,
	PLD_VERTICAL_POSITION_LOWER
};

enum acpi_pld_horizontal_position {
	PLD_HORIZONTAL_POSITION_LEFT,
	PLD_HORIZONTAL_POSITION_CENTER,
	PLD_HORIZONTAL_POSITION_RIGHT
};

enum acpi_pld_shape {
	PLD_SHAPE_ROUND,
	PLD_SHAPE_OVAL,
	PLD_SHAPE_SQUARE,
	PLD_SHAPE_VERTICAL_RECTANGLE,
	PLD_SHAPE_HORIZONTAL_RECTANGLE,
	PLD_SHAPE_VERTICAL_TRAPEZOID,
	PLD_SHAPE_HORIZONTAL_TRAPEZOID,
	PLD_SHAPE_UNKNOWN,
	PLD_SHAPE_CHAMFERED
};

enum acpi_pld_orientation {
	PLD_ORIENTATION_HORIZONTAL,
	PLD_ORIENTATION_VERTICAL,
};

enum acpi_pld_rotate {
	PLD_ROTATE_0,
	PLD_ROTATE_45,
	PLD_ROTATE_90,
	PLD_ROTATE_135,
	PLD_ROTATE_180,
	PLD_ROTATE_225,
	PLD_ROTATE_270,
	PLD_ROTATE_315
};

#define ACPI_PLD_GROUP(__token, __position)	\
	{					\
		.token = __token,		\
		.position = __position,		\
	}

/*
 * ACPI specification 6.3 third paragraph of section 6.1.8:
 * All Panel references (Top, Bottom, Right, Left, etc.) are interpreted
 * as though the user is facing the front of the system.
 *
 * A `_PLD` describes the offset and rotation of a single device connection point
 * from an `origin` that resides in the lower left hand corner of its Panel.
 */

#define ACPI_PLD_TYPE_A(__panel, __horiz, __grp)				\
	{									\
		.visible = true,						\
		.panel = PLD_PANEL_##__panel,					\
		.shape = PLD_SHAPE_HORIZONTAL_RECTANGLE,			\
		.horizontal_position = PLD_HORIZONTAL_POSITION_##__horiz,	\
		.group = __grp,							\
	}

#define ACPI_PLD_TYPE_C(__panel, __horiz, __grp)				\
	{									\
		.visible = true,						\
		.panel = PLD_PANEL_##__panel,					\
		.shape = PLD_SHAPE_OVAL,					\
		.horizontal_position = PLD_HORIZONTAL_POSITION_##__horiz,	\
		.group = __grp,							\
	}

struct acpi_pld_group {
	uint8_t token;
	uint8_t position;
};

struct acpi_pld {
	/* Color field can be explicitly ignored */
	bool ignore_color;
	uint8_t color_red;
	uint8_t color_blue;
	uint8_t color_green;

	/* Port characteristics */
	bool visible;		/* Can be seen by the user */
	bool lid;		/* Port is on lid of device */
	bool dock;		/* Port is in a docking station */
	bool bay;		/* Port is in a bay */
	bool ejectable;		/* Device is ejectable, has _EJx objects */
	bool ejectable_ospm;	/* Device needs OSPM to eject */
	uint16_t width;		/* Width in mm */
	uint16_t height;	/* Height in mm */
	uint16_t vertical_offset;
	uint16_t horizontal_offset;
	enum acpi_pld_panel panel;
	enum acpi_pld_horizontal_position horizontal_position;
	enum acpi_pld_vertical_position vertical_position;
	enum acpi_pld_shape shape;
	enum acpi_pld_rotate rotation;

	/* Port grouping */
	enum acpi_pld_orientation orientation;
	struct acpi_pld_group group;
	uint8_t draw_order;
	uint8_t cabinet_number;
	uint8_t card_cage_number;

	/* Set if this PLD defines a reference shape */
	bool reference_shape;
};

/* Fill out PLD structure with defaults based on USB port type */
int acpi_pld_fill_usb(struct acpi_pld *pld, enum acpi_upc_type type,
		      struct acpi_pld_group *group);

/* Turn PLD structure into a 20 byte ACPI buffer */
int acpi_pld_to_buffer(const struct acpi_pld *pld, uint8_t *buf, int buf_len);

#endif /* __ACPI_ACPI_PLD_H__ */
