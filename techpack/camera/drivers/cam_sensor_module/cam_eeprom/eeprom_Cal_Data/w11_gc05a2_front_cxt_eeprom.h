struct eeprom_memory_map_init_write_params w11_gc05a2_front_cxt_eeprom  ={
    .slave_addr = 0x6e,
          .mem_settings =
          {
            {0x0315, CAMERA_SENSOR_I2C_TYPE_WORD,0x80, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
            {0x031c, CAMERA_SENSOR_I2C_TYPE_WORD,0x60, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
            {0x0af4, CAMERA_SENSOR_I2C_TYPE_WORD,0x01, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
            {0x0af6, CAMERA_SENSOR_I2C_TYPE_WORD,0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
            {0x0b90, CAMERA_SENSOR_I2C_TYPE_WORD,0x10, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
            {0x0b91, CAMERA_SENSOR_I2C_TYPE_WORD,0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
            {0x0b92, CAMERA_SENSOR_I2C_TYPE_WORD,0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
            {0x0ba0, CAMERA_SENSOR_I2C_TYPE_WORD,0x17, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
            {0x0ba1, CAMERA_SENSOR_I2C_TYPE_WORD,0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
            {0x0ba2, CAMERA_SENSOR_I2C_TYPE_WORD,0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
            {0x0ba4, CAMERA_SENSOR_I2C_TYPE_WORD,0x03, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
            {0x0ba5, CAMERA_SENSOR_I2C_TYPE_WORD,0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
            {0x0ba6, CAMERA_SENSOR_I2C_TYPE_WORD,0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
            {0x0ba8, CAMERA_SENSOR_I2C_TYPE_WORD,0x40, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
            {0x0ba9, CAMERA_SENSOR_I2C_TYPE_WORD,0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
            {0x0baa, CAMERA_SENSOR_I2C_TYPE_WORD,0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
            {0x0bac, CAMERA_SENSOR_I2C_TYPE_WORD,0x40, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
            {0x0bad, CAMERA_SENSOR_I2C_TYPE_WORD,0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
            {0x0bae, CAMERA_SENSOR_I2C_TYPE_WORD,0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
            {0x0bb0, CAMERA_SENSOR_I2C_TYPE_WORD,0x02, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
            {0x0bb1, CAMERA_SENSOR_I2C_TYPE_WORD,0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
            {0x0bb2, CAMERA_SENSOR_I2C_TYPE_WORD,0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
            {0x0bb8, CAMERA_SENSOR_I2C_TYPE_WORD,0x02, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
            {0x0bb9, CAMERA_SENSOR_I2C_TYPE_WORD,0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
            {0x0bba, CAMERA_SENSOR_I2C_TYPE_WORD,0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
            {0x0a70, CAMERA_SENSOR_I2C_TYPE_WORD,0x80, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
            {0x0a71, CAMERA_SENSOR_I2C_TYPE_WORD,0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
            {0x0a72, CAMERA_SENSOR_I2C_TYPE_WORD,0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
            {0x0a66, CAMERA_SENSOR_I2C_TYPE_WORD,0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
            {0x0a67, CAMERA_SENSOR_I2C_TYPE_WORD,0x80, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
            {0x0a4d, CAMERA_SENSOR_I2C_TYPE_WORD,0x0e, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
            {0x0a45, CAMERA_SENSOR_I2C_TYPE_WORD,0x02, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
            {0x0a47, CAMERA_SENSOR_I2C_TYPE_WORD,0x02, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
            {0x0a50, CAMERA_SENSOR_I2C_TYPE_WORD,0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
            {0x0a4f, CAMERA_SENSOR_I2C_TYPE_WORD,0x0c, CAMERA_SENSOR_I2C_TYPE_BYTE, 10},

            {0x0a67, CAMERA_SENSOR_I2C_TYPE_WORD,0x84, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
            {0x0a69, CAMERA_SENSOR_I2C_TYPE_WORD,0x20, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
            {0x0a6a, CAMERA_SENSOR_I2C_TYPE_WORD,0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
            {0x0a66, CAMERA_SENSOR_I2C_TYPE_WORD,0x20, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
            {0x0a66, CAMERA_SENSOR_I2C_TYPE_WORD,0x12, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
          },
          .memory_map_size = 40,
};

struct eeprom_memory_map_init_write_params w11_gc05a2_front_cxt_eeprom_after_read  ={
    .slave_addr = 0x6e,
          .mem_settings =
          {
            {0x0a70, CAMERA_SENSOR_I2C_TYPE_WORD,0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
            {0x0a67, CAMERA_SENSOR_I2C_TYPE_WORD,0x00, CAMERA_SENSOR_I2C_TYPE_BYTE, 0},
          },
          .memory_map_size = 2,
};

struct  WingtechOtpCheckInfo w11_gc05a2_front_cxt_otp_checkinfo ={
    .groupInfo =
    {
        .IsAvailable = TRUE,
        .CheckItemOffset =
        { 0x0, 0x41, 0x0, 0x69 }, //group offset
        .GroupFlag =
        { 0x01, 0x07, 0x1f }, //group flag
    },

    .ItemInfo =
    {
        { { TRUE, 0x1, 0x8 }, { TRUE, 0x9, 0x8 }, { TRUE, 0x11, 0x8 }, }, //module info
        { { TRUE, 0x42, 0xd }, { TRUE, 0x4f, 0xd }, { TRUE, 0x5c, 0xd }, }, //awb info
        { { FALSE, 0x0224, 6 }, {FALSE, 0x0937, 6 }, { FALSE, 0x104A, 6 }, }, //af info
        { { TRUE, 0x6a, 0x6e9 }, { TRUE, 0x753, 0x6e9 }, { TRUE, 0xe3c, 0x6e9 }, },//lsc info
        //{ { TRUE, 88, 8, 8 }, { TRUE, 88, 8, 8 }, { TRUE, 88, 8, 8 }, }, //pdaf info
    },
};