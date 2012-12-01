{
    "variables": {
        "foo": 220
    },
    "display_video": {
        "driver": "video",
        "layers": 3,
        "foreground": "FFFFFFFF",
        "background": "00000000",
        "fill": 1,
        "cols": 256,
        "rows": 64,
        "layout-timeout": 0,
        "update": 100,
        //"widget0": "widget_key_up",
        //"widget1": "widget_key_down",
        //"widget2": "widget_netstat_script",
        "transition-speed": 100,
        "refresh-rate": 5,
        "layout0": "layout_full"
    },
    "layout_full": {
        "layer4": {
            "row1": {
                "col1": "widget_infinite_256x64"
            }
        },
        "layer3": {
            "row1": {
                "col1": "widget_libvisual_256x64"
            }
        },
        "layer2": {
            "row1": {
                "col1": "widget_histogram_large"
            }
        },
        "layer1": {
            "row2": {
                "col1": "widget_icon_heart",
                "col2": "widget_icon_heartbeat",
                "col3": "widget_icon_heart",
                "col4": "widget_icon_heartbeat",
                "col5": "widget_icon_heart",
                "col6": "widget_icon_heartbeat",
                "col7": "widget_icon_heart",
                "col8": "widget_icon_heartbeat",
                "col9": "widget_icon_heart",
                "col10": "widget_icon_heartbeat",
                "col11": "widget_icon_heart",
                "col12": "widget_icon_heartbeat",
                "col13": "widget_icon_heart",
                "col14": "widget_icon_heartbeat",
                "col15": "widget_icon_heart",
                "col16": "widget_icon_heartbeat",
                "col17": "widget_icon_heart",
                "col18": "widget_icon_heartbeat",
                "col19": "widget_icon_heart",
                "col20": "widget_icon_heartbeat"
            },
            "row3": {
                "col4": "widget_cpu_label",
                "col8": "widget_bignums",
                "col11": "widget_percent"
            },
            "row5": {
                "col1": "widget_FSSpace"
            },
            "transition": "U"
        }
    },
    "layout_2x16": {
        "row1": {
                "col1": "widget_CPULabel",
                "col5": "widget_CPU"
        },
        "row2": {
                "col1": "widget_page_label",
                "col11": "widget_page"
        }
    },
    "layout_histogram_large": {
        "row1": {
            "col1": "widget_histogram_large"
        },
        "transition": "U"
    },
    "layout_full_256x64": {
        "layer2": {
            "row1": {
                "col1": "widget_histogram_large"
            }
        },
        "layer3": {
            "row1": {
                "col1": "widget_libvisual"
            }
        },
        "row1": {
            "col1": "widget_CPULabel",
            "col6": "widget_CPU",
            "col11": "widget_CPUBar",
            "col21": "widget_RAMLabel",
            "col26": "widget_RAMFree",
            "col31": "widget_RAMTotal"
        },
        "row2": {
            "col1": "widget_IDELabel",
            "col6": "widget_IDEIn",
            "col17": "widget_IDEOut",
            "col28": "widget_IDEBar"
        },
        "row3": {
            "col1": "widget_FSSpace"
        },
        "row4": {
            "col1": "widget_WLANLabel",
            "col6": "widget_WLANIn",
            "col17": "widget_WLANOut",
            "col28": "widget_WLANBar"
        },
        "row6": {
            "col1": "widget_icon_heartbeat",
            "col2": "widget_icon_ekg",
            "col3": "widget_icon_heart",
            "col4": "widget_icon_blob",
            "col5": "widget_icon_wave",
            "col6": "widget_icon_timer",
            "col7": "widget_icon_rain",
            "col8": "widget_icon_karo",
            "col9": "widget_icon_heartbeat",
            "col10": "widget_icon_ekg",
            "col11": "widget_icon_heart",
            "col12": "widget_icon_blob",
            "col13": "widget_icon_wave",
            "col14": "widget_icon_timer",
            "col15": "widget_icon_rain",
            "col16": "widget_icon_karo",
            "col17": "widget_icon_heartbeat",
            "col18": "widget_icon_ekg",
            "col19": "widget_icon_heart",
            "col20": "widget_icon_blob",
            "col21": "widget_icon_wave",
            "col22": "widget_icon_timer",
            "col23": "widget_icon_rain",
            "col24": "widget_icon_karo",
            "col25": "widget_icon_heartbeat",
            "col26": "widget_icon_ekg",
            "col27": "widget_icon_heart",
            "col28": "widget_icon_blob",
            "col29": "widget_icon_wave",
            "col30": "widget_icon_timer",
            "col31": "widget_icon_rain",
            "col32": "widget_icon_karo",
            "col33": "widget_icon_heartbeat",
            "col34": "widget_icon_ekg",
            "col35": "widget_icon_heart",
            "col36": "widget_icon_blob",
            "col37": "widget_icon_blob",
            "col38": "widget_icon_wave",
            "col39": "widget_icon_timer",
            "col40": "widget_icon_rain",
            "col41": "widget_icon_karo",
            "col42": "widget_icon_heartbeat"
            
        },
        "row7": {
            "col1": "widget_Time",
            "col22": "widget_UptimeLabel",
            "col30": "widget_Uptime"
        },
        "row8": {
            "col1": "widget_bottom_ticker"
        },
        "transition": "U"
    },
    "layout_blank": {
        "layout-timeout": 0
    },
    "layout_pcm_256x64": {
        "row1": {
            "col1": "widget_pcm_256x64"
        },
        "transition": "U"
    },
    "layout_pcm_24x16": {
        "row1": {
            "col1": "widget_pcm_24x16"
        }
    },
    "layout_peak": {
        "row1": {
            "col1": "widget_peak"
        }
    },
    "layout_spectrum": {
        "row1": {
            "col1": "widget_spectrum"
        },
        "transition": "U"
    },
    "layout_1": {
        "row1": {
            "col1": "widget_cpu_label",
            //"col11": "widget_cpu_histogram",
            "col6": "widget_cpu"
        }, 
        "row2": {
            //"col1": "widget_wlan0_label", 
            //"col11": "widget_wlan0_histogram",
            //"col7": "widget_wlan0"
        }, 
        "row3": {
           //"col1": "widget_ram_label",
           //"col5": "widget_ram_active",
           //"col11": "widget_ram_total"
        }, 
        "row4": {
            //"col1": "widget_bottom_ticker"
        }, 
        "transition": "B"
    }, 
    "layout_visualization": {
        "row1": {
            "col1": "widget_pcm_24x16"
            //"col17": "widget_pcm_24x16",
            //"col9": "widget_pcm_24x16"
        },
        "row3": {
            //"col13": "widget_pcm_24x16",
            //"col5": "widget_pcm_24x16"
        },
        "transition": "B"
    },
    "layout_american_flag": {
        "row1": {
            "col1": "widget_gif_american_flag",
            "col17": "widget_gif_american_flag", 
            "col9": "widget_gif_american_flag"
        }, 
        "row3": {
            "col13": "widget_gif_american_flag", 
            "col5": "widget_gif_american_flag"
        },
        "transition": "D"
    }, 
    "layout_bar": {
        "row1": {
            "col1": "widget_ram_label",
            "col10": "widget_ram_total", 
            "col5": "widget_ram_active"
        },
        "row2": {
            "col1": "widget_wlan0_label",
            "col10": "widget_wlan0_bar",
            "col7": "widget_wlan0"
        }, 
        "row3": {
            "col1": "widget_icon_wave",
            "col10": "widget_icon_ekg", 
            "col11": "widget_icon_rain", 
            "col12": "widget_icon_blob", 
            "col13": "widget_icon_wave", 
            "col14": "widget_icon_ekg", 
            "col15": "widget_icon_rain", 
            "col16": "widget_icon_blob", 
            "col17": "widget_icon_wave", 
            "col18": "widget_icon_ekg", 
            "col19": "widget_icon_rain",
            "col2": "widget_icon_ekg", 
            "col20": "widget_icon_blob", 
            "col3": "widget_icon_rain", 
            "col4": "widget_icon_blob", 
            "col5": "widget_icon_wave", 
            "col6": "widget_icon_ekg", 
            "col7": "widget_icon_rain", 
            "col8": "widget_icon_blob", 
            "col9": "widget_icon_wave"
        },
        "row4": {
            "col1": "widget_bottom_ticker"
        }, 
        "transition": "U"
    }, 
    "layout_bignums": {
        "row2": {
            "col1": "widget_cpu_label", 
            "col10": "widget_bignums"
        }, 
        "row3": {
            "col14": "widget_percent"
        },
        "transition": "D"
    }, 
    "layout_banner": {
        "row1": {
                "col2": "widget_vis_banner",
                "col12": "widget_vis_banner"
        },
        "row2": {
                "col2": "widget_vis_banner",
                "col12": "widget_vis_banner"
        },
        "row3": {
            "col1": "widget_Time"

        },
        "row4": {
                "col2": "widget_vis_banner",
                "col12": "widget_vis_banner"
        },
        "transition": "U"
    },
    "layout_bolt": {
        "row1": {
            "col1": "widget_gif_bolt", 
            "col17": "widget_gif_bolt", 
            "col9": "widget_gif_bolt"
        }, 
        "row3": {
            "col13": "widget_gif_bolt", 
            "col5": "widget_gif_bolt"
        },
        "transition": "B"
    }, 
    "layout_dolby": {
        "row1": {
            "col1": "widget_gif_dolby", 
            "col17": "widget_gif_dolby", 
            "col9": "widget_gif_dolby"
        }, 
        "row3": {
            "col13": "widget_gif_dolby", 
            "col5": "widget_gif_dolby"
        },
        "transition": "U"
    }, 
    "layout_icons": {
        "row1": {
            "col1": "widget_icon_heartbeat", 
            "col10": "widget_icon_ekg", 
            "col11": "widget_icon_heart", 
            "col12": "widget_icon_blob", 
            "col13": "widget_icon_wave", 
            "col14": "widget_icon_timer", 
            "col15": "widget_icon_rain", 
            "col16": "widget_icon_karo", 
            "col17": "widget_icon_heartbeat", 
            "col18": "widget_icon_ekg", 
            "col19": "widget_icon_heart", 
            "col2": "widget_icon_ekg", 
            "col20": "widget_icon_blob", 
            "col3": "widget_icon_heart", 
            "col4": "widget_icon_blob", 
            "col5": "widget_icon_wave", 
            "col6": "widget_icon_timer", 
            "col7": "widget_icon_rain", 
            "col8": "widget_icon_karo", 
            "col9": "widget_icon_heartbeat"
        }, 
        "row2": {
            "col1": "widget_icon_wave", 
            "col10": "widget_icon_timer", 
            "col11": "widget_icon_rain", 
            "col12": "widget_icon_karo", 
            "col13": "widget_icon_heartbeat", 
            "col14": "widget_icon_ekg", 
            "col15": "widget_icon_heart", 
            "col16": "widget_icon_blob", 
            "col17": "widget_icon_wave", 
            "col18": "widget_icon_timer", 
            "col19": "widget_icon_rain", 
            "col2": "widget_icon_timer", 
            "col20": "widget_icon_karo", 
            "col3": "widget_icon_rain", 
            "col4": "widget_icon_karo", 
            "col5": "widget_icon_heartbeat", 
            "col6": "widget_icon_ekg", 
            "col7": "widget_icon_heart", 
            "col8": "widget_icon_blob", 
            "col9": "widget_icon_wave"
        }, 
        "row3": {
            "col1": "widget_icon_heartbeat", 
            "col10": "widget_icon_ekg", 
            "col11": "widget_icon_heart", 
            "col12": "widget_icon_blob", 
            "col13": "widget_icon_wave", 
            "col14": "widget_icon_timer", 
            "col15": "widget_icon_rain", 
            "col16": "widget_icon_karo", 
            "col17": "widget_icon_heartbeat", 
            "col18": "widget_icon_ekg", 
            "col19": "widget_icon_heart", 
            "col2": "widget_icon_ekg", 
            "col20": "widget_icon_blob", 
            "col3": "widget_icon_heart", 
            "col4": "widget_icon_blob", 
            "col5": "widget_icon_wave", 
            "col6": "widget_icon_timer", 
            "col7": "widget_icon_rain", 
            "col8": "widget_icon_karo", 
            "col9": "widget_icon_heartbeat"
        }, 
        "row4": {
            "col1": "widget_icon_wave", 
            "col10": "widget_icon_timer", 
            "col11": "widget_icon_rain", 
            "col12": "widget_icon_karo", 
            "col13": "widget_icon_heartbeat", 
            "col14": "widget_icon_ekg", 
            "col15": "widget_icon_heart", 
            "col16": "widget_icon_blob", 
            "col17": "widget_icon_wave", 
            "col18": "widget_icon_timer", 
            "col19": "widget_icon_rain", 
            "col2": "widget_icon_timer", 
            "col20": "widget_icon_karo", 
            "col3": "widget_icon_rain", 
            "col4": "widget_icon_karo", 
            "col5": "widget_icon_heartbeat", 
            "col6": "widget_icon_ekg", 
            "col7": "widget_icon_heart", 
            "col8": "widget_icon_blob", 
            "col9": "widget_icon_wave"
        }, 
        "transition": "B"
    }, 
    "layout_intel": {
        "row1": {
            "col1": "widget_gif_intel", 
            "col17": "widget_gif_intel", 
            "col9": "widget_gif_intel"
        }, 
        "row3": {
            "col13": "widget_gif_intel", 
            "col5": "widget_gif_intel"
        },
        "transition": "R"
    }, 
    "layout_karo": {
        "row1": {
            "col1": "widget_icon_karo", 
            "col10": "widget_icon_karo", 
            "col11": "widget_icon_karo", 
            "col12": "widget_icon_karo", 
            "col13": "widget_icon_karo", 
            "col14": "widget_icon_karo", 
            "col15": "widget_icon_karo", 
            "col16": "widget_icon_karo", 
            "col17": "widget_icon_karo", 
            "col18": "widget_icon_karo", 
            "col19": "widget_icon_karo", 
            "col2": "widget_icon_karo", 
            "col20": "widget_icon_karo", 
            "col3": "widget_icon_karo", 
            "col4": "widget_icon_karo", 
            "col5": "widget_icon_karo", 
            "col6": "widget_icon_karo", 
            "col7": "widget_icon_karo", 
            "col8": "widget_icon_karo", 
            "col9": "widget_icon_karo"
        }, 
        "row2": {
            "col1": "widget_icon_karo", 
            "col10": "widget_icon_karo", 
            "col11": "widget_icon_karo", 
            "col12": "widget_icon_karo", 
            "col13": "widget_icon_karo", 
            "col14": "widget_icon_karo", 
            "col15": "widget_icon_karo", 
            "col16": "widget_icon_karo", 
            "col17": "widget_icon_karo", 
            "col18": "widget_icon_karo", 
            "col19": "widget_icon_karo", 
            "col2": "widget_icon_karo", 
            "col20": "widget_icon_karo", 
            "col3": "widget_icon_karo", 
            "col4": "widget_icon_karo", 
            "col5": "widget_icon_karo", 
            "col6": "widget_icon_karo", 
            "col7": "widget_icon_karo", 
            "col8": "widget_icon_karo", 
            "col9": "widget_icon_karo"
        }, 
        "row3": {
            "col1": "widget_icon_karo", 
            "col10": "widget_icon_karo", 
            "col11": "widget_icon_karo", 
            "col12": "widget_icon_karo", 
            "col13": "widget_icon_karo", 
            "col14": "widget_icon_karo", 
            "col15": "widget_icon_karo", 
            "col16": "widget_icon_karo", 
            "col17": "widget_icon_karo", 
            "col18": "widget_icon_karo", 
            "col19": "widget_icon_karo", 
            "col2": "widget_icon_karo", 
            "col20": "widget_icon_karo", 
            "col3": "widget_icon_karo", 
            "col4": "widget_icon_karo", 
            "col5": "widget_icon_karo", 
            "col6": "widget_icon_karo", 
            "col7": "widget_icon_karo", 
            "col8": "widget_icon_karo", 
            "col9": "widget_icon_karo"
        }, 
        "row4": {
            "col1": "widget_icon_karo", 
            "col10": "widget_icon_karo", 
            "col11": "widget_icon_karo", 
            "col12": "widget_icon_karo", 
            "col13": "widget_icon_karo", 
            "col14": "widget_icon_karo", 
            "col15": "widget_icon_karo", 
            "col16": "widget_icon_karo", 
            "col17": "widget_icon_karo", 
            "col18": "widget_icon_karo", 
            "col19": "widget_icon_karo", 
            "col2": "widget_icon_karo", 
            "col20": "widget_icon_karo", 
            "col3": "widget_icon_karo", 
            "col4": "widget_icon_karo", 
            "col5": "widget_icon_karo", 
            "col6": "widget_icon_karo", 
            "col7": "widget_icon_karo", 
            "col8": "widget_icon_karo", 
            "col9": "widget_icon_karo"
        }, 
        "transition": "D"
    }, 
    "layout_marquee1": {
        "row1": {
            "col1": "widget_gif_marquee1", 
            "col11": "widget_gif_marquee1", 
            "col13": "widget_gif_marquee1", 
            "col15": "widget_gif_marquee1", 
            "col17": "widget_gif_marquee1", 
            "col19": "widget_gif_marquee1", 
            "col3": "widget_gif_marquee1", 
            "col5": "widget_gif_marquee1", 
            "col7": "widget_gif_marquee1", 
            "col9": "widget_gif_marquee1"
        }, 
        "row2": {
            "col1": "widget_gif_marquee1", 
            "col12": "widget_wlan0_short_bar", 
            "col19": "widget_gif_marquee1", 
            "col3": "widget_wlan0_label", 
            "col9": "widget_wlan0"
        }, 
        "row3": {
            "col1": "widget_gif_marquee1", 
            "col19": "widget_gif_marquee1", 
            "col3": "widget_bottom_marquee"
        }, 
        "row4": {
            "col1": "widget_gif_marquee1", 
            "col11": "widget_gif_marquee1", 
            "col13": "widget_gif_marquee1", 
            "col15": "widget_gif_marquee1", 
            "col17": "widget_gif_marquee1", 
            "col19": "widget_gif_marquee1", 
            "col3": "widget_gif_marquee1", 
            "col5": "widget_gif_marquee1", 
            "col7": "widget_gif_marquee1", 
            "col9": "widget_gif_marquee1"
        },
        "transition": "B"
    }, 
    "layout_marquee2": {
        "row1": {
            "col1": "widget_gif_marquee2", 
            "col11": "widget_gif_marquee2", 
            "col13": "widget_gif_marquee2", 
            "col15": "widget_gif_marquee2", 
            "col17": "widget_gif_marquee2", 
            "col19": "widget_gif_marquee2", 
            "col3": "widget_gif_marquee2", 
            "col5": "widget_gif_marquee2", 
            "col7": "widget_gif_marquee2", 
            "col9": "widget_gif_marquee2"
        }, 
        "row2": {
            "col1": "widget_gif_marquee2", 
            "col12": "widget_wlan0_short_bar", 
            "col19": "widget_gif_marquee2", 
            "col3": "widget_wlan0_label", 
            "col9": "widget_wlan0"
        }, 
        "row3": {
            "col1": "widget_gif_marquee2", 
            "col19": "widget_gif_marquee2", 
            "col3": "widget_bottom_marquee"
        }, 
        "row4": {
            "col1": "widget_gif_marquee2", 
            "col11": "widget_gif_marquee2", 
            "col13": "widget_gif_marquee2", 
            "col15": "widget_gif_marquee2", 
            "col17": "widget_gif_marquee2", 
            "col19": "widget_gif_marquee2", 
            "col3": "widget_gif_marquee2", 
            "col5": "widget_gif_marquee2", 
            "col7": "widget_gif_marquee2", 
            "col9": "widget_gif_marquee2"
        },
        "transition": "U"
    }, 
    "layout_marquee3": {
        "row1": {
            "col1": "widget_gif_marquee3", 
            "col11": "widget_gif_marquee3", 
            "col13": "widget_gif_marquee3", 
            "col15": "widget_gif_marquee3", 
            "col17": "widget_gif_marquee3", 
            "col19": "widget_gif_marquee3", 
            "col3": "widget_gif_marquee3", 
            "col5": "widget_gif_marquee3", 
            "col7": "widget_gif_marquee3", 
            "col9": "widget_gif_marquee3"
        }, 
        "row2": {
            "col1": "widget_gif_marquee3", 
            "col12": "widget_wlan0_short_bar", 
            "col19": "widget_gif_marquee3", 
            "col3": "widget_wlan0_label", 
            "col9": "widget_wlan0"
        }, 
        "row3": {
            "col1": "widget_gif_marquee3", 
            "col19": "widget_gif_marquee3", 
            "col3": "widget_bottom_marquee"
        }, 
        "row4": {
            "col1": "widget_gif_marquee3", 
            "col11": "widget_gif_marquee3", 
            "col13": "widget_gif_marquee3", 
            "col15": "widget_gif_marquee3", 
            "col17": "widget_gif_marquee3", 
            "col19": "widget_gif_marquee3", 
            "col3": "widget_gif_marquee3", 
            "col5": "widget_gif_marquee3", 
            "col7": "widget_gif_marquee3", 
            "col9": "widget_gif_marquee3"
        },
        "transition": "D"
    }, 
    "layout_marquee4": {
        "row1": {
            "col1": "widget_gif_marquee4", 
            "col11": "widget_gif_marquee4", 
            "col13": "widget_gif_marquee4", 
            "col15": "widget_gif_marquee4", 
            "col17": "widget_gif_marquee4", 
            "col19": "widget_gif_marquee4", 
            "col3": "widget_gif_marquee4", 
            "col5": "widget_gif_marquee4", 
            "col7": "widget_gif_marquee4", 
            "col9": "widget_gif_marquee4"
        }, 
        "row2": {
            "col1": "widget_gif_marquee4", 
            "col12": "widget_wlan0_short_bar", 
            "col19": "widget_gif_marquee4", 
            "col3": "widget_wlan0_label", 
            "col9": "widget_wlan0"
        }, 
        "row3": {
            "col1": "widget_gif_marquee4", 
            "col19": "widget_gif_marquee4", 
            "col3": "widget_bottom_marquee"
        }, 
        "row4": {
            "col1": "widget_gif_marquee4", 
            "col11": "widget_gif_marquee4", 
            "col13": "widget_gif_marquee4", 
            "col15": "widget_gif_marquee4", 
            "col17": "widget_gif_marquee4", 
            "col19": "widget_gif_marquee4", 
            "col3": "widget_gif_marquee4", 
            "col5": "widget_gif_marquee4", 
            "col7": "widget_gif_marquee4", 
            "col9": "widget_gif_marquee4"
        },
        "transition": "U"
    }, 
    "plugins": {
        "i2c_sensors-path": "/sys/bus/i2c/devices/1-0028/"
    }, 
    "widget_superscope": {
        "init": [
  "n=500",
  "r=10",
  "mx=0",
  "my=0",
  "mz=0",
  "dst=0.2",
  "rx=0",
  "ry=0",
  "rz=0",
  "rdx=1",
  "rdy=1",
  "rdz=1",
  "p=3.14159265",
  "p2=10.0*p",
  "p3=50/p"

        ],
        "frame": [
  "rx=rx+rdx",
  "ry=ry+rdy",
  "rz=rz+rdz",
  "xs=sin(rx/p3)",
  "ys=sin(ry/p3)",
  "zs=sin(rz/p3)",
  "xc=cos(rx/p3)",
  "yc=cos(ry/p3)",
  "zc=cos(rz/p3)"
        ],
        "beat": [
  "r=rand(8) + 2"
        ],
        "point": [
  "x1=v*sin(p2*i)",
  "y1=v * i / cos(z1)",
  "z1=r*cos(p2*i)",
  "y2=y1*xc-z1*xs",
  "z2=y1*xs+z1*xc",
  "x2=z2*ys+x1*yc",
  "z3=z2*yc-x1*ys",
  "x3=x2*zc-y2*zs",
  "y3=y2*zc+x2*zs",
  "x4=mx+x3",
  "y4=my+y3",
  "z4=mz+z3",
  "x=x4/(1+z4/dst)",
  "y=y4/(1+z4/dst)"

        ]
    },
    "widget_page_label": {
        "type": "text",
        "length": 10,
        "expression": "return 'PAGE FILE:'"
    },
    "widged_page": {
        "type": "text",
        "length": 6,
        "expression": "return procstat.ProcStat('page')"
    },
    "widget_histogram_large": {
        "type": "histogram",
        "expression": "return procstat.Cpu('busy', 500)",
        "foreground": "99005500",
        "background": "77777777",
        "height": 32,
        "width": 160,
        "update": 100
    },
    "widget_hddtemp": {
        "type": "text",
        "expression": "return 'IDE Temp: ' .. exec.Exec('hddtemp /dev/sda1 | cut -f 3 -d :', 999)",
        "length": 14,
        "align": "L",
        "update": 999
    },
    "widget_FSSpace": {
        "type": "text",
        "expression": "fsspace_a = ((statfs.Statfs('/', 'bavail')*statfs.Statfs('/', 'bsize'))/ 1024/ 1024);fsspace_b = ((statfs.Statfs('/', 'bavail')*statfs.Statfs('/', 'bsize'))/1024/1024);c = '/ ' .. fsspace_a .. 'MB / ' .. fsspace_b .. ' MB'; return c;
        ",
        "length": 42,
        "align": "M",
        "speed": 30
    },
    "widget_CPU": {
        "type": "text",
        "expression": "return procstat.Cpu('busy', 500)",
        "postfix": "return '% '",
        "length": 5,
        "precision": 1,
        "align": "R",
        "update": 50
    },
    "widget_CPUBar": {
        "type": "bar",
        "expression": "return procstat.Cpu('busy', 500)",
        "expression2": "return procstat.Cpu('system', 500)",
        "length": 10,
        "min": "1",
        "max": "100",
        "direction": "E",
        "style": "H",
        "update": 50
    },
    "widget_CPULabel": {
        "type": "text",
        "expression": "return 'CPU:'",
        "length": 4,
        "align": "L",
        "bold": 1
    },
    "widget_RAMTotal": {
        "type": "text",
        "expression": "return meminfo.Meminfo('MemTotal')/1024",
        "postfix": "return 'MB FREE'",
        "length": 11,
        "precision": 0,
        "align": "L",
        "update": 50
    },
    "widget_RAMFree": {
        "type": "text",
        "expression": "return meminfo.Meminfo('MemFree')/1024",
        "postfix": "return '/'",
        "length": 5,
        "precision": 0,
        "align": "R",
        "update": 50
    },
    "widget_RAMLabel": {
        "type": "text",
        "expression": "return 'RAM:'",
        "length": 4,
        "align": "L",
        "bold": 1
    },
    "widget_IDEIn": {
        "type": "text",
        "expression": "return diskstats.Diskstats('sda', 'read_sectors', 100)/2048",
        "prefix": "return 'In '",
        "postfix": "return 'MB'",
        "precision": 2,
        "align": "R",
        "length": 10,
        "update": 500
    },
    "widget_IDEOut": {
        "type": "text",
        "expression": "return diskstats.Diskstats('sda', 'write_sectors', 100)/2048",
        "prefix": "return 'Out'",
        "postfix": "return 'MB'",
        "precision": 2,
        "align": "R",
        "length": 10,
        "update": 500
    },
    "widget_IDEBar": {
        "type": "bar",
        "expression": "return diskstats.Diskstats('sda', 'read_sectors', 100)",
        "expression2": "return diskstats.Diskstats('sda', 'write_sectors', 100)",
        "length": 14,
        "direction": "E",
        "style": "H",
        "update": 500
    },
    "widget_IDELabel": {
        "type": "text",
        "expression": "return 'IDE:'",
        "length": 4,
        "align": "L",
        "bold": 1
    },
    "widget_WLANLabel": {
        "type": "text",
        "expression": "return 'WLAN:'",
        "length": 5,
        "align": "L",
        "bold": 1
    },
    "widget_WLANIn": {
        "type": "text",
        "expression": "return netdev.Fast('wlan0', 'Rx_bytes', 500)/1024",
        "prefix": "'OUT'",
        "postfix": "'KB'",
        "length": 9,
        "precision": 0,
        "align": "R",
        "update": 50
    },
    "widget_WLANOut": {
        "type": "text",
        "expression": "return netdev.Fast('wlan0', 'Tx_bytes', 500)/1024",
        "prefix": "'IN'",
        "postfix": "'KB'",
        "length": 9,
        "precision": 0,
        "align": "R",
        "update": 50
    },
    "widget_WLANBar": {
        "type": "bar",
        "expression": "return netdev.Fast('wlan0', 'Rx_bytes', 500)",
        "expression2": "return netdev.Fast('wlan0', 'Tx_bytes', 500)",
        "length": 14,
        "direction": "E",
        "style": "H",
        "update": 50
    },
    "widget_TimeLabel": {
        "type": "text",
        "expression": "return 'TIME:'",
        "length": 5,
        "align": "L",
        "bold": 1
    },
    "widget_Time": {
        "type": "text",
        "expression": "return time.Strftime('%a,%m/%d %H:%M:%S', time.Time())",
        "length": 20,
        "align": "C",
        "update": 10
    },
    "widget_UptimeLabel": {
        "type": "text",
        "expression": "return 'UPTIME:'",
        "length": 7,
        "align": "L",
        "bold": 1
    },
    "widget_Uptime": {
        "type": "text",
        "expression": "return uptime.Uptime('%d d %H:%M:%S')",
        "length": 21,
        "align": "L",
        "update": 999
    },
    "widget_fps": {
        "type": "text",
        "expression": "return fps.avg() .. ' - FPS'",
        "length": 11,
        "foreground": "FFFFFFFF",
        "background": "ff000000"
    },
    "widget_vis_banner": {
        "type": "filter",
        "width": 48,
        "height": 16,
        "update": 100,
        "actor": "dmovement",
        "style": "pcm"
    },
    "widget_infinite_256x64": {
        "type": "visualization",
        "width": 256,
        "height": 64,
        "update": 50,
        "morph-timeout": 0,
        "morph-steps": 10,
        "skip-actors": "gstreamer:gdkpixbuf:lcdcontrol:jess:oinksie:goom2k4:jakdaw",
        "actor": "gforce",
        "input": "debug",
        "morph": "slide_up",
        "style": "pcm",
        "alpha": 255
    },
    "widget_libvisual_256x64": {
        "type": "visualization",
        "width": 256,
        "height": 64,
        "update": 50,
        "morph-timeout": 0,
        "morph-steps": 18,
        "skip-actors": "gstreamer:gdkpixbuf:lcdcontrol:jess:oinksie:goom2k4:jakdaw",
        "actor": "lv_analyzer",
        "input": "debug",
        "morph": "tentacle",
        "style": "pcm",
        "alpha": 50,
        "params": {
            "oinksie": {
                "acid palette": 1,
                "color mode": 4
            },
            /*"blursk": { // blur scope
                "show_info": "Always",
                "show_timestamp": 1,
                "color": "r=246,g=242,b=20",
                "color_style": "Dimming",
                "signal_color": "Normal signal",
                "contour_lines": 0,
                "hue_on_beats": 0,
                "fade_speed": "Medium fade",
                "blur_style": "Simple",
                "transition_speed": "Medium switch",
                "blur_when": "Full blur",
                "blur_stencil": "No stencil",
                "slow_motion": 1,
                "signal_style": "Oscilloscope",
                "plot_style": "Line",
                "thick_on_beats": 1,
                "flash_style": "No flash",
                "overall_effect": "Normal effect",
                "random_dots": "FALSE",
                "background": "Black bkgnd",
                "floaters": "No floaters",
                "beat_sensitivity": 4
            },*/
            "blursk": { // box spectrum
                "color": "r=0,g=254,b=0",
                "color_style": "Flame",
                "signal_color": "White signal",
                "contour_lines": 0,
                "hue_on_beats": 0,
                "fade_speed": "Slow fade",
                "blur_style": "Backward",
                "transition_speed": "Medium switch",
                "blur_when": "Reduced blur",
                "blur_stencil": "No stencil",
                "slow_motion": 0,
                "signal_style": "Stereo spectrum",
                "plot_style": "Edges",
                "thick_on_beats": 1,
                "flash_style": "No flash",
                "overall_effect": "Normal effect",
                "random_dots": "FALSE",
                "background": "Dark bkgnd",
                "floaters": "No floaters"
            },
            /*"blursk": { // burning logo
                "color": "r=254,g=98,b=38",
                "color_style": "Flame",
                "signal_color": "Normal signal",
                "contour_lines": 0,
                "hue_on_beats": 0,
                "fade_speed": "Medium fade",
                "blur_style": "Cylinder",
                "transition_speed": "Slow switch",
                "blur_when": "Reduced blur",
                "blur_stencil": "Blursk stencil",
                "slow_motion": 1,
                "signal_style": "Stereo spectrum",
                "plot_style": "Airbrush",
                "thick_on_beats": 1,
                "flash_style": "Blursk flash",
                "overall_effect": "Normal effect",
                "random_dots": "TRUE",
                "background": "Black bkgnd",
                "floaters": "No floaters"
            },*/
            /*"blursk": { // cat scan
                "color": "r=175,g=245,b=0",
                "color_style": "Rainbow",
                "signal_color": "Normal signal",
                "contour_lines": 0,
                "hue_on_beats": 0,
                "fade_speed": "Fast fade",
                "blur_style": "Four way",
                "transition_speed": "Fast switch",
                "blur_when": "Smear",
                "blur_stencil": "No stencil",
                "slow_motion": 1,
                "signal_style": "Radial spectrum",
                "plot_style": "Airbrush",
                "thick_on_beats": 1,
                "flash_style": "No flash",
                "overall_effect": "Ripple effect",
                "random_dots": "FALSE",
                "background": "Black bkgnd",
                "floaters": "No floaters"
            },*/
            /*"blursk": { // cubism
                "color": "r=0,g=254,b=254",
                "color_style": "Color stripes",
                "signal_color": "Normal signal",
                "contour_lines": 0,
                "hue_on_beats": 0,
                "fade_speed": "Medium fade",
                "blur_style": "Tangram",
                "transition_speed": "Medium switch",
                "blur_when": "Melt",
                "blur_stencil": "No stencil",
                "slow_motion": 0,
                "signal_style": "Mono spectrum",
                "plot_style": "Mirrored",
                "thick_on_beats": 1,
                "flash_style": "Invert flash",
                "overall_effect": "Anti-fade effect",
                "random_dots": "FALSE",
                "background": "Dark bkgnd",
                "floaters": "No floaters"
            },
            "blursk": { // fan
                "color": "r=0,g=255,b=0",
                "color_style": "Milky",
                "signal_color": "Normal signal",
                "contour_lines": 0,
                "hue_on_beats": 1,
                "fade_speed": "Slow fade",
                "blur_style": "Four way",
                "transition_speed": "Medium switch",
                "blur_when": "No blur",
                "blur_stencil": "Tux stencil",
                "slow_motion": 1,
                "signal_style": "Mono spectrum",
                "plot_style": "Radar",
                "thick_on_beats": 1,
                "flash_style": "No flash",
                "overall_effect": "Normal effect",
                "random_dots": "FALSE",
                "background": "Black bkgnd",
                "floaters": "No floaters"
            },
            "blursk": { // flag
                "color": "r=0,g=255,b=255",
                "color_style": "Color bands",
                "signal_color": "Normal signal",
                "contour_lines": 0,
                "hue_on_beats": 0,
                "fade_speed": "Slow fade",
                "blur_style": "Random",
                "transition_speed": "Medium switch",
                "blur_when": "Reduced blur",
                "blur_stencil": "No stencil",
                "slow_motion": 1,
                "signal_style": "Mono spectrum",
                "plot_style": "Radar",
                "thick_on_beats": 0,
                "flash_style": "No flash",
                "overall_effect": "Normal effect",
                "random_dots": "FALSE",
                "background": "Black bkgnd",
                "floaters": "No floaters"
            },
            "blursk": { // flame scan
                "color": "r=242,g=74,b=0",
                "color_style": "Flame",
                "signal_color": "White signal",
                "contour_lines": 0,
                "hue_on_beats": 0,
                "fade_speed": "Slow fade",
                "blur_style": "Flow around",
                "transition_speed": "Medium switch",
                "blur_when": "Full blur",
                "blur_stencil": "No stencil",
                "slow_motion": 0,
                "signal_style": "Stereo spectrum",
                "plot_style": "Edges",
                "thick_on_beats": 1,
                "flash_style": "No flash",
                "overall_effect": "Ripple effect",
                "random_dots": "FALSE",
                "background": "Black bkgnd",
                "floaters": "No floaters"
            },
            "blursk": { // flashbulb
                "color": "r=0,g=178,b=246",
                "color_style": "Dimming",
                "signal_color": "Normal signal",
                "contour_lines": 1,
                "hue_on_beats": 1,
                "fade_speed": "Medium fade",
                "blur_style": "Prismatic",
                "transition_speed": "Medium switch",
                "blur_when": "Melt",
                "blur_stencil": "No stencil",
                "slow_motion": 1,
                "signal_style": "Mono spectrum",
                "plot_style": "None",
                "thick_on_beats": 1,
                "flash_style": "Random flash",
                "overall_effect": "Normal effect",
                "random_dots": "FALSE",
                "background": "Black bkgnd",
                "floaters": "No floaters"
            },
            "blursk": { // foil
                "color": "r=0,g=254,b=254",
                "color_style": "Layers",
                "signal_color": "Normal signal",
                "contour_lines": 0,
                "hue_on_beats": 0,
                "fade_speed": "Medium fade",
                "blur_style": "Forward",
                "transition_speed": "Medium switch",
                "blur_when": "Reduced blur",
                "blur_stencil": "No stencil",
                "slow_motion": 0,
                "signal_style": "Flower",
                "plot_style": "Line",
                "thick_on_beats": 1,
                "flash_style": "No flash",
                "overall_effect": "Normal effect",
                "random_dots": "FALSE",
                "background": "Black bkgnd",
                "floaters": "No floaters"
            },
            "blursk": { // frenetic
                "color": "r=0,g=254,b=254",
                "color_style": "Graying",
                "signal_color": "Normal signal",
                "contour_lines": 0,
                "hue_on_beats": 0,
                "fade_speed": "Fast fade",
                "blur_style": "Fast forward",
                "transition_speed": "Medium switch",
                "blur_when": "Smear",
                "blur_stencil": "No stencil",
                "slow_motion": 0,
                "signal_style": "Phase shift",
                "plot_style": "Line",
                "thick_on_beats": 0,
                "flash_style": "No flash",
                "overall_effect": "Normal effect",
                "random_dots": "FALSE",
                "background": "Flash bkgnd",
                "floaters": "No floaters"
            },
            "blursk": { // heat wave
                "color": "r=0,g=254,b=254",
                "color_style": "Color layers",
                "signal_color": "Normal signal",
                "contour_lines": 0,
                "hue_on_beats": 0,
                "fade_speed": "Slow fade",
                "blur_style": "Rise",
                "transition_speed": "Medium switch",
                "blur_when": "Reduced blur",
                "blur_stencil": "No stencil",
                "slow_motion": 0,
                "signal_style": "Stereo spectrum",
                "plot_style": "Line",
                "thick_on_beats": 1,
                "flash_style": "No flash",
                "overall_effect": "Normal effect",
                "random_dots": "FALSE",
                "black_background": "TRUE",
                "toggle_bpl": "Edge smooth",
                "background": "White bkgnd",
                "floaters": "No floaters"
            },
            "blursk": { // interference
                "color": "r=0,g=254,b=254",
                "color_style": "Color stripes",
                "signal_color": "Normal signal",
                "contour_lines": 1,
                "hue_on_beats": 0,
                "fade_speed": "Medium fade",
                "blur_style": "Forward",
                "transition_speed": "Medium switch",
                "blur_when": "Full blur",
                "blur_stencil": "No stencil",
                "slow_motion": 0,
                "signal_style": "Flower",
                "plot_style": "Mirrored",
                "thick_on_beats": 0,
                "flash_style": "No flash",
                "overall_effect": "Normal effect",
                "random_dots": "FALSE",
                "background": "Black bkgnd",
                "floaters": "No floaters"
            },
            "blursk": { // lens
                "color": "r=249,g=69,b=0",
                "color_style": "Milky",
                "signal_color": "Normal signal",
                "contour_lines": 0,
                "hue_on_beats": 1,
                "fade_speed": "Medium fade",
                "blur_style": "Ripple",
                "transition_speed": "Medium switch",
                "blur_when": "Full blur",
                "blur_stencil": "No stencil",
                "slow_motion": 1,
                "signal_style": "Mono spectrum",
                "plot_style": "Mirrored",
                "thick_on_beats": 1,
                "flash_style": "No flash",
                "overall_effect": "Normal effect",
                "random_dots": "FALSE",
                "background": "Black bkgnd",
                "floaters": "No floaters"
            },
            "blursk": { // metalic
                "color": "r=0,g=253,b=253",
                "color_style": "Metal",
                "signal_color": "Normal signal",
                "contour_lines": 0,
                "hue_on_beats": 0,
                "fade_speed": "Medium fade",
                "blur_style": "Random slow",
                "transition_speed": "Medium switch",
                "blur_when": "Smear",
                "blur_stencil": "No stencil",
                "slow_motion": 1,
                "signal_style": "Oscilloscope",
                "plot_style": "Line",
                "thick_on_beats": 1,
                "flash_style": "No flash",
                "overall_effect": "Bump effect",
                "random_dots": "FALSE",
                "black_background": "TRUE",
                "toggle_bpl": "Area smooth",
                "background": "Black bkgnd",
                "floaters": "No floaters"
            },
            "blursk": { // nucleus
                "color": "r=11,g=253,b=249",
                "color_style": "Color stripes",
                "signal_color": "Normal signal",
                "contour_lines": 1,
                "hue_on_beats": 0,
                "fade_speed": "Medium fade",
                "blur_style": "Fast forward",
                "transition_speed": "Medium switch",
                "blur_when": "Full blur",
                "blur_stencil": "No stencil",
                "slow_motion": 1,
                "signal_style": "Phase shift",
                "plot_style": "Fuzzy dots",
                "thick_on_beats": 0,
                "flash_style": "No flash",
                "overall_effect": "Normal effect",
                "random_dots": "FALSE",
                "black_background": "TRUE",
                "toggle_bpl": "Edge smooth",
                "background": "Black bkgnd",
                "floaters": "No floaters"
            },
            "blursk": { // quick change
                "color": "r=250,g=129,b=253",
                "color_style": "Standoff",
                "signal_color": "Normal signal",
                "contour_lines": 0,
                "hue_on_beats": 0,
                "fade_speed": "Medium fade",
                "blur_style": "Random",
                "transition_speed": "Fast switch",
                "blur_when": "Smear",
                "blur_stencil": "No stencil",
                "slow_motion": 0,
                "signal_style": "Mono spectrum",
                "plot_style": "Radar",
                "thick_on_beats": 1,
                "flash_style": "No flash",
                "overall_effect": "Normal effect",
                "random_dots": "FALSE",
                "toggle_bpl": "Area smooth",
                "background": "Black bkgnd",
                "floaters": "No floaters"
            },
            "blursk": { // random flower
                "color": "r=0,g=254,b=254",
                "color_style": "Random",
                "signal_color": "Normal signal",
                "contour_lines": 0,
                "hue_on_beats": 0,
                "fade_speed": "Medium fade",
                "blur_style": "Random slow",
                "transition_speed": "Medium switch",
                "blur_when": "Full blur",
                "blur_stencil": "No stencil",
                "slow_motion": 0,
                "signal_style": "Flower",
                "plot_style": "Line",
                "thick_on_beats": 1,
                "flash_style": "No flash",
                "overall_effect": "Normal effect",
                "random_dots": "FALSE",
                "background": "Random bkgnd",
                "floaters": "No floaters"
            },
            "blursk": { // random mirrored spectrum
                "color": "r=252,g=154,b=10",
                "color_style": "Random",
                "signal_color": "Normal signal",
                "contour_lines": 0,
                "hue_on_beats": 0,
                "fade_speed": "Medium fade",
                "blur_style": "Random slow",
                "transition_speed": "Medium switch",
                "blur_when": "Full blur",
                "blur_stencil": "No stencil",
                "slow_motion": 0,
                "signal_style": "Mono spectrum",
                "plot_style": "Mirrored",
                "thick_on_beats": 1,
                "flash_style": "Random flash",
                "overall_effect": "Normal effect",
                "random_dots": "FALSE",
                "background": "Random bkgnd",
                "toggle_bpl": "Area smooth",
                "floaters": "No floaters"
            },
            "blursk": { // ripple
                "color": "r=0,g=254,b=254",
                "color_style": "Color stripes",
                "signal_color": "Normal signal",
                "contour_lines": 0,
                "hue_on_beats": 0,
                "fade_speed": "Medium fade",
                "blur_style": "Random",
                "transition_speed": "Medium switch",
                "blur_when": "Full blur",
                "blur_stencil": "No stencil",
                "slow_motion": 1,
                "signal_style": "High/Low plot",
                "plot_style": "Starburst",
                "thick_on_beats": 1,
                "flash_style": "No flash",
                "overall_effect": "Normal effect",
                "random_dots": "FALSE",
                "background": "Shift bkgnd",
                "floaters": "No floaters"
            },
            "blursk": { // roller
                "color": "r=70,g=0,b=251",
                "color_style": "Stripes",
                "signal_color": "Normal signal",
                "contour_lines": 1,
                "hue_on_beats": 0,
                "fade_speed": "Medium fade",
                "blur_style": "Cylinder",
                "transition_speed": "Fast switch",
                "blur_when": "Smear",
                "blur_stencil": "No stencil",
                "slow_motion": 1,
                "signal_style": "Stereo spectrum",
                "plot_style": "Airbrush",
                "thick_on_beats": 1,
                "flash_style": "No flash",
                "overall_effect": "Anti-fade effect",
                "random_dots": "FALSE",
                "background": "Black bkgnd",
                "toggle_bpl": "Area smooth",
                "floaters": "No floaters"
            },
            "blursk": { // rose
                "color": "r=0,g=254,b=254",
                "color_style": "Rainbow",
                "signal_color": "Normal signal",
                "contour_lines": 0,
                "hue_on_beats": 0,
                "fade_speed": "Medium fade",
                "blur_style": "Tangram",
                "transition_speed": "Slow switch",
                "blur_when": "Smear",
                "blur_stencil": "No stencil",
                "slow_motion": 0,
                "signal_style": "Flower",
                "plot_style": "Line",
                "thick_on_beats": 1,
                "flash_style": "No flash",
                "overall_effect": "Anti-fade effect",
                "random_dots": "FALSE",
                "background": "White bkgnd",
                "floaters": "No floaters"
            },
            "blursk": { // scan
                "color": "r=175,g=245,b=0",
                "color_style": "Rainbow",
                "signal_color": "Normal signal",
                "contour_lines": 0,
                "hue_on_beats": 0,
                "fade_speed": "Medium fade",
                "blur_style": "Flow around",
                "transition_speed": "Medium switch",
                "blur_when": "Full blur",
                "blur_stencil": "No stencil",
                "slow_motion": 0,
                "signal_style": "Stereo spectrum",
                "plot_style": "None",
                "thick_on_beats": 1,
                "flash_style": "Invert flash",
                "overall_effect": "Anti-fade effect",
                "random_dots": "TRUE",
                "background": "Black bkgnd",
                "floaters": "Slow floaters"
            },
            "blursk": { // sparkler
                "color": "r=252,g=72,b=0",
                "color_style": "Metal",
                "signal_color": "Normal signal",
                "contour_lines": 0,
                "hue_on_beats": 1,
                "fade_speed": "Medium fade",
                "blur_style": "Four way",
                "transition_speed": "Medium switch",
                "blur_when": "Smear",
                "blur_stencil": "No stencil",
                "slow_motion": 1,
                "signal_style": "Mono spectrum",
                "plot_style": "Radar",
                "thick_on_beats": 1,
                "flash_style": "No flash",
                "overall_effect": "Normal effect",
                "random_dots": "FALSE",
                "background": "Black bkgnd",
                "floaters": "No floaters"
            },
            "blursk": { // subliminal logo
                "color": "r=175,g=245,b=0",
                "color_style": "Random",
                "signal_color": "Normal signal",
                "contour_lines": 0,
                "hue_on_beats": 0,
                "fade_speed": "Slow fade",
                "blur_style": "Random",
                "transition_speed": "Medium switch",
                "blur_when": "Random blur",
                "blur_stencil": "Random stencil",
                "slow_motion": 1,
                "signal_style": "Flower",
                "plot_style": "Line",
                "thick_on_beats": 0,
                "flash_style": "Random flash",
                "overall_effect": "Ripple effect",
                "random_dots": "FALSE",
                "background": "Black bkgnd",
                "floaters": "No floaters"
            },
            "blursk": { // tunnel
                "color": "r=179,g=251,b=0",
                "color_style": "Color layers",
                "signal_color": "Normal signal",
                "contour_lines": 0,
                "hue_on_beats": 0,
                "fade_speed": "Slow fade",
                "blur_style": "Forward",
                "transition_speed": "Medium switch",
                "blur_when": "Reduced blur",
                "blur_stencil": "No stencil",
                "slow_motion": 0,
                "signal_style": "Flower",
                "plot_style": "Line",
                "thick_on_beats": 1,
                "flash_style": "No flash",
                "overall_effect": "Normal effect",
                "random_dots": "FALSE",
                "background": "Black bkgnd",
                "floaters": "No floaters"
            },
            "blursk": { // tunnel2
                "color": "r=245,g=68,b=0",
                "color_style": "Color layers",
                "signal_color": "Normal signal",
                "contour_lines": 0,
                "hue_on_beats": 1,
                "fade_speed": "Medium fade",
                "blur_style": "Fast forward",
                "transition_speed": "Medium switch",
                "blur_when": "Full blur",
                "blur_stencil": "No stencil",
                "slow_motion": 1,
                "signal_style": "Flower",
                "plot_style": "Line",
                "thick_on_beats": 1,
                "flash_style": "No flash",
                "overall_effect": "Normal effect",
                "random_dots": "FALSE",
                "background": "Dark bkgnd",
                "floaters": "No floaters"
            },
            "blursk": { // wiggler
                "color": "r=0,g=253,b=253",
                "color_style": "Color layers",
                "signal_color": "Normal signal",
                "contour_lines": 0,
                "hue_on_beats": 0,
                "fade_speed": "Slow fade",
                "blur_style": "Flow around",
                "transition_speed": "Fast switch",
                "blur_when": "Full blur",
                "blur_stencil": "No stencil",
                "slow_motion": 0,
                "signal_style": "Radial spectrum",
                "plot_style": "Starburst",
                "thick_on_beats": 1,
                "flash_style": "No flash",
                "overall_effect": "Normal effect",
                "random_dots": "FALSE",
                "background": "Dark bkgnd",
                "floaters": "No floaters"
            },
            "blursk": { // xmms
                "color": "r=0,g=254,b=254",
                "color_style": "Dimming",
                "signal_color": "Normal signal",
                "contour_lines": 0,
                "hue_on_beats": 0,
                "fade_speed": "Slow fade",
                "blur_style": "Random",
                "transition_speed": "Medium switch",
                "blur_when": "Reduced blur",
                "blur_stencil": "XMMS stencil",
                "slow_motion": 1,
                "signal_style": "Flower",
                "plot_style": "Line",
                "thick_on_beats": 0,
                "flash_style": "XMMS flash",
                "overall_effect": "Normal effect",
                "random_dots": "FALSE",
                "background": "Color bkgnd",
                "floaters": "No floaters"
            },
            "blursk": { // bump scope
                "color": "r=246,g=240,b=10",
                "color_style": "Dimming",
                "signal_color": "Normal signal",
                "contour_lines": 0,
                "hue_on_beats": 0,
                "fade_speed": "Medium fade",
                "blur_style": "Simple",
                "transition_speed": "Medium switch",
                "blur_when": "Full blur",
                "blur_stencil": "No stencil",
                "slow_motion": 1,
                "signal_style": "Oscilloscope",
                "plot_style": "Line",
                "thick_on_beats": 1,
                "flash_style": "No flash",
                "overall_effect": "Bump effect",
                "random_dots": "FALSE",
                "background": "Black bkgnd",
                "floaters": "No floaters"
            },
            "blursk": { // wax
                "color": "r=38,g=246,b=9",
                "color_style": "Flame",
                "signal_color": "Normal signal",
                "contour_lines": 0,
                "hue_on_beats": 1,
                "fade_speed": "Fast fade",
                "blur_style": "Gravity",
                "transition_speed": "Medium switch",
                "blur_when": "Smear",
                "blur_stencil": "No stencil",
                "slow_motion": 0,
                "signal_style": "Oscilloscope",
                "plot_style": "Line",
                "thick_on_beats": 1,
                "flash_style": "No flash",
                "overall_effect": "Anti-fade effect",
                "random_dots": "FALSE",
                "background": "White bkgnd",
                "floaters": "No floaters"
            },
            "blursk": { // gas cloud
                "color": "r=247,g=128,b=0",
                "color_style": "Color layers",
                "signal_color": "Normal signal",
                "contour_lines": 0,
                "hue_on_beats": 0,
                "fade_speed": "Medium fade",
                "blur_style": "Flow between",
                "transition_speed": "Fast switch",
                "blur_when": "Smear",
                "blur_stencil": "No stencil",
                "slow_motion": 0,
                "signal_style": "Radial spectrum",
                "plot_style": "Starburst",
                "thick_on_beats": 1,
                "flash_style": "No flash",
                "overall_effect": "Ripple effect",
                "random_dots": "FALSE",
                "background": "Shift bkgnd",
                "floaters": "No floaters"
            },
            "blursk": { // aurora
                "color": "r=251,g=205,b=2",
                "color_style": "Color standoff",
                "signal_color": "Normal signal",
                "contour_lines": 0,
                "hue_on_beats": 0,
                "fade_speed": "Medium fade",
                "blur_style": "Forward",
                "transition_speed": "Medium switch",
                "blur_when": "Full blur",
                "blur_stencil": "No stencil",
                "slow_motion": 0,
                "signal_style": "Radial spectrum",
                "plot_style": "Inertia",
                "thick_on_beats": 0,
                "flash_style": "No flash",
                "overall_effect": "Normal effect",
                "random_dots": "FALSE",
                "background": "Black bkgnd",
                "floaters": "No floaters"
            },
            "blursk": { // fractal drums
                "color": "r=255,g=234,b=0",
                "color_style": "Brightening",
                "signal_color": "Normal signal",
                "contour_lines": 0,
                "hue_on_beats": 0,
                "fade_speed": "Medium fade",
                "blur_style": "Fractal sphere",
                "transition_speed": "Medium switch",
                "blur_when": "Full blur",
                "blur_stencil": "No stencil",
                "slow_motion": 1,
                "signal_style": "Phase shift",
                "plot_style": "Fuzzy dots",
                "thick_on_beats": 1,
                "flash_style": "No flash",
                "overall_effect": "Bump effect",
                "random_dots": "FALSE",
                "toggle_bpl": "Edge smooth",
                "background": "Black bkgnd",
                "floaters": "No floaters"
            },
            "blursk": { // butterfly
                "color": "r=0,g=254,b=0",
                "color_style": "Color standoff",
                "signal_color": "White signal",
                "contour_lines": 0,
                "hue_on_beats": 0,
                "fade_speed": "Slow fade",
                "blur_style": "Backward",
                "transition_speed": "Medium switch",
                "blur_when": "Smear",
                "blur_stencil": "No stencil",
                "slow_motion": 0,
                "signal_style": "Stereo spectrum",
                "plot_style": "Edges",
                "thick_on_beats": 1,
                "flash_style": "No flash",
                "overall_effect": "Normal effect",
                "random_dots": "FALSE",
                "background": "Shift bkgnd",
                "floaters": "No floaters"
            },
            "blursk": { // radioactive bob
                "color": "r=248,g=248,b=0",
                "color_style": "Milky",
                "signal_color": "Normal signal",
                "contour_lines": 0,
                "hue_on_beats": 1,
                "black_background": "FALSE",
                "fade_speed": "Medium fade",
                "blur_style": "Tangram",
                "transition_speed": "Medium switch",
                "blur_when": "Full blur",
                "blur_stencil": "Bob stencil",
                "slow_motion": 1,
                "toggle_bpl": "Area smooth",
                "signal_style": "Phase shift",
                "plot_style": "None",
                "thick_on_beats": 1,
                "flash_style": "Bob flash",
                "overall_effect": "Normal effect",
                "random_dots": "TRUE",
                "background": "Color bkgnd",
                "floaters": "No floaters"
            },
            "blursk": { // testing 1 2 3
                "color": "r=237,g=5,b=230",
                "color_style": "Flame",
                "signal_color": "Normal signal",
                "contour_lines": 0,
                "hue_on_beats": 0,
                "background": "Random bkgnd",
                "fade_speed": "Slow fade",
                "blur_style": "Random slow",
                "transition_speed": "Medium switch",
                "blur_when": "Full blur",
                "blur_stencil": "Maybe stencil",
                "slow_motion": 0,
                "toggle_bpl": "Area smooth",
                "signal_style": "Mono spectrum",
                "plot_style": "Inertia",
                "thick_on_beats": 0,
                "flash_style": "No flash",
                "overall_effect": "Normal effect",
                "floaters": "Fast floaters"
            },
            "blursk": { // drain
                "color": "r=228,g=150,b=0",
                "color_style": "Rainbow",
                "signal_color": "Normal signal",
                "contour_lines": 0,
                "hue_on_beats": 0,
                "background": "Color bkgnd",
                "fade_speed": "Slow fade",
                "blur_style": "Drain",
                "transition_speed": "Medium switch",
                "blur_when": "Reduced blur",
                "blur_stencil": "No stencil",
                "slow_motion": 0,
                "signal_style": "Stereo spectrum",
                "plot_style": "Edges",
                "thick_on_beats": 1,
                "flash_style": "No flash",
                "overall_effect": "Anti-fade effect",
                "floaters": "No floaters"
            },*/
            "none": {}
        }
    },
    "widget_pcm_24x16": {
        "type": "visualization",
        "width": 24,
        "height": 16,
        "update": 100,
        "style": "pcm",
        "actor": "oinksie",
        "morph-timeout": 0,
        "skip-actors": "gstreamer:gdkpixbuf:jakdaw"
    },
    "widget_peak": {
        "type": "visualization",
        "width": 20,
        "height": 4,
        "update": 100,
        "style": "peak"
    },
    "widget_spectrum": {
        "type": "visualization",
        "width": 20,
        "height": 4,
        "update": 100,
        "style": "spectrum"
    },
    "widget_bignums": {
        "expression": "return procstat.Cpu('busy', 200) ", 
        "type": "bignums", 
        "max": 100,
        "min": 0,
        "update": 1,
        "foreground": "ffFFFFFF",
        "background": "00ffffff"
    }, 
    "widget_cpuinfo_label": {
        "expression": "return 'CPUInfo:  '",
        "type": "text",
        "length": 10
    },
    "widget_cpuinfo": {
        "expression": "return cpuinfo.Cpuinfo('model name') .. '     '",
        "type": "text",
        "length": 10
    },
    "widget_bottom_marquee": {
        "align": "M", 
        "expression": "uname.Uname('sysname') + ' ' + uname.Uname('nodename') + ' ' + uname.Uname('release') + ' ' + cpuinfo.Cpuinfo('model name')", 
        "type": "text", 
        "speed": 100, 
        "length": 16
    }, 
    "widget_bottom_ticker": {
        "align": "P", 
        "expression": "uname.Uname('sysname') + ' ' + uname.Uname('nodename') + ' ' + uname.Uname('release') + ' ' + cpuinfo.Cpuinfo('model name')", 
        "type": "text", 
        "speed": 100,
        "bold": 1,
        "length": 42
    }, 
    "widget_percent": {
        "expression": "return '%'",
        "length": 1
    },
    "widget_cpu": {
        "align": "R", 
        "expression": "procstat.Cpu('busy', 500)", 
        "postfix": "'%'", 
        "precision": 0, 
        "type": "text", 
        "update": 500, 
        "length": 5
    }, 
    "widget_cpu_bar": {
        "direction": "W", 
        "expression": "procstat.Cpu('busy', 500)", 
        "length": 10, 
        "max": "100", 
        "min": "0", 
        "style": "H", 
        "type": "bar", 
        "update": 500
    }, 
    "widget_cpu_histogram": {
        "direction": "E", 
        "expression": "procstat.Cpu('busy', 500)", 
        "gap": 1, 
        "length": 10, 
        "max": "100", 
        "min": "0", 
        "type": "histogram", 
        "update": 500
    }, 
    "widget_cpu_label": {
        "align": "L", 
        "expression": "return 'CPU:'", 
        "type": "text", 
        "length": 4
    }, 
    "widget_dow": {
        "align": "L", 
        "expression": "", 
        "type": "text"
    }, 
    "widget_dow_end": {
        "align": "L", 
        "expression": "'\\200'", 
        "type": "text", 
        "length": 1
    }, 
    "widget_dow_label": {
        "align": "L", 
        "expression": "'Ambient:'", 
        "type": "text", 
        "length": 10
    }, 
    "widget_gif_american_flag": {
        "file": "gifs/American_Flag_ani.gif", 
        "type": "gif", 
        "update": "250", 
        "xpoint": 1,
        "inverted": 1
    }, 
    "widget_gif_bolt": {
        "file": "gifs/lightning_bolt_ani.gif", 
        "type": "gif", 
        "update": "250",
        "inverted": 1
    }, 
    "widget_gif_intel": {
        "file": "gifs/intel_logo_ani.gif", 
        "start": 1, 
        "end": 6, 
        "type": "gif", 
        "update": "250",
        "inverted": 1
    }, 
    "widget_gif_marquee1": {
        "file": "gifs/marquee1.gif", 
        "height": 8, 
        "type": "gif", 
        "update": "250", 
        "width": 12,
        "height": 12,
        "inverted": 1
    }, 
    "widget_gif_marquee2": {
        "file": "gifs/marquee2.gif", 
        "type": "gif", 
        "update": "250",
        "widget": 12,
        "height": 12,
        "inverted": 1
    }, 
    "widget_gif_marquee3": {
        "file": "gifs/marquee3.gif", 
        "type": "gif", 
        "update": "250",
        "widget": 12,
        "height": 12,
        "inverted": 1
    }, 
    "widget_gif_marquee4": {
        "file": "gifs/marquee4.gif", 
        "type": "gif", 
        "update": "250",
        "widget": 12,
        "height": 12,
        "inverted": 1
    }, 
    "widget_icon_blob": {
        "bitmap": {
            "row1": ".....|.....|.....", 
            "row2": ".....|.....|.***.", 
            "row3": ".....|.***.|*...*", 
            "row4": "..*..|.*.*.|*...*", 
            "row5": ".....|.***.|*...*", 
            "row6": ".....|.....|.***.", 
            "row7": ".....|.....|.....", 
            "row8": ".....|.....|....."
        }, 
        "speed": 300, 
        "type": "icon"
    }, 
    "widget_icon_ekg": {
        "bitmap": {
            "row1": ".....|.....|.....|.....|.....|.....|.....|.....", 
            "row2": ".....|....*|...*.|..*..|.*...|*....|.....|.....", 
            "row3": ".....|....*|...*.|..*..|.*...|*....|.....|.....", 
            "row4": ".....|....*|...**|..**.|.**..|**...|*....|.....", 
            "row5": ".....|....*|...**|..**.|.**..|**...|*....|.....", 
            "row6": ".....|....*|...*.|..*.*|.*.*.|*.*..|.*...|*....", 
            "row7": "*****|*****|****.|***..|**..*|*..**|..***|.****", 
            "row8": ".....|.....|.....|.....|.....|.....|.....|....."
        }, 
        "speed": 30, 
        "type": "icon"
    }, 
    "widget_icon_heart": {
        "bitmap": {
            "row1": ".....|.....|.....|.....|.....|.....", 
            "row2": ".*.*.|.....|.*.*.|.....|.....|.....", 
            "row3": "*****|.*.*.|*****|.*.*.|.*.*.|.*.*.", 
            "row4": "*****|.***.|*****|.***.|.***.|.***.", 
            "row5": ".***.|.***.|.***.|.***.|.***.|.***.", 
            "row6": ".***.|..*..|.***.|..*..|..*..|..*..", 
            "row7": "..*..|.....|..*..|.....|.....|.....", 
            "row8": ".....|.....|.....|.....|.....|....."
        }, 
        "speed": 500, 
        "type": "icon"
    }, 
    "widget_icon_heartbeat": {
        "bitmap": {
            "row1": ".....|.....", 
            "row2": ".*.*.|.*.*.", 
            "row3": "*****|*.*.*", 
            "row4": "*****|*...*", 
            "row5": ".***.|.*.*.", 
            "row6": ".***.|.*.*.", 
            "row7": "..*..|..*..", 
            "row8": ".....|....."
        }, 
        "speed": 500, 
        "type": "icon"
    }, 
    "widget_icon_karo": {
        "bitmap": {
            "row1": ".....|.....|.....|.....|..*..|.....|.....|.....", 
            "row2": ".....|.....|.....|..*..|.*.*.|..*..|.....|.....", 
            "row3": ".....|.....|..*..|.*.*.|*...*|.*.*.|..*..|.....", 
            "row4": ".....|..*..|.*.*.|*...*|.....|*...*|.*.*.|..*..", 
            "row5": ".....|.....|..*..|.*.*.|*...*|.*.*.|..*..|.....", 
            "row6": ".....|.....|.....|..*..|.*.*.|..*..|.....|.....", 
            "row7": ".....|.....|.....|.....|..*..|.....|.....|.....", 
            "row8": ".....|.....|.....|.....|.....|.....|.....|....."
        }, 
        "speed": 100, 
        "type": "icon"
    }, 
    "widget_icon_rain": {
        "bitmap": {
            "row1": "...*.|.....|.....|.*...|....*|..*..|.....|*....", 
            "row2": "*....|...*.|.....|.....|.*...|....*|..*..|.....", 
            "row3": ".....|*....|...*.|.....|.....|.*...|....*|..*..", 
            "row4": "..*..|.....|*....|...*.|.....|.....|.*...|....*", 
            "row5": "....*|..*..|.....|*....|...*.|.....|.....|.*...", 
            "row6": ".*...|....*|..*..|.....|*....|...*.|.....|.....", 
            "row7": ".....|.*...|....*|..*..|.....|*....|...*.|.....", 
            "row8": ".....|.....|.*...|....*|..*..|.....|*....|...*."
        }, 
        "speed": 100, 
        "type": "icon"
    }, 
    "widget_icon_squirrel": {
        "bitmap": {
            "row1": ".....|.....|.....|.....|.....|.....", 
            "row2": ".....|.....|.....|.....|.....|.....", 
            "row3": ".....|.....|.....|.....|.....|.....", 
            "row4": "**...|.**..|..**.|...**|....*|.....", 
            "row5": "*****|*****|*****|*****|*****|*****", 
            "row6": "...**|..**.|.**..|**...|*....|.....", 
            "row7": ".....|.....|.....|.....|.....|.....", 
            "row8": ".....|.....|.....|.....|.....|....."
        }, 
        "speed": 100, 
        "type": "icon"
    }, 
    "widget_icon_timer": {
        "bitmap": {
            "row1": ".....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|", 
            "row2": ".***.|.*+*.|.*++.|.*++.|.*++.|.*++.|.*++.|.*++.|.*++.|.*++.|.*++.|.*++.|.+++.|.+*+.|.+**.|.+**.|.+**.|.+**.|.+**.|.+**.|.+**.|.+**.|.+**.|.+**.|", 
            "row3": "*****|**+**|**++*|**+++|**++.|**++.|**+++|**+++|**+++|**+++|**+++|+++++|+++++|++*++|++**+|++***|++**.|++**.|++***|++***|++***|++***|++***|*****|", 
            "row4": "*****|**+**|**+**|**+**|**+++|**+++|**+++|**+++|**+++|**+++|+++++|+++++|+++++|++*++|++*++|++*++|++***|++***|++***|++***|++***|++***|*****|*****|", 
            "row5": "*****|*****|*****|*****|*****|***++|***++|**+++|*++++|+++++|+++++|+++++|+++++|+++++|+++++|+++++|+++++|+++**|+++**|++***|+****|*****|*****|*****|", 
            "row6": ".***.|.***.|.***.|.***.|.***.|.***.|.**+.|.*++.|.+++.|.+++.|.+++.|.+++.|.+++.|.+++.|.+++.|.+++.|.+++.|.+++.|.++*.|.+**.|.***.|.***.|.***.|.***.|", 
            "row7": ".....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|", 
            "row8": ".....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|.....|"
        }, 
        "speed":  100, 
        "type": "icon"
    }, 
    "widget_icon_wave": {
        "bitmap": {
            "row1": "..**.|.**..|**...|*....|.....|.....|.....|.....|....*|...**", 
            "row2": ".*..*|*..*.|..*..|.*...|*....|.....|.....|....*|...*.|..*..", 
            "row3": "*....|....*|...*.|..*..|.*...|*....|....*|...*.|..*..|.*...", 
            "row4": "*....|....*|...*.|..*..|.*...|*....|....*|...*.|..*..|.*...", 
            "row5": "*....|....*|...*.|..*..|.*...|*....|....*|...*.|..*..|.*...", 
            "row6": ".....|.....|....*|...*.|..*..|.*..*|*..*.|..*..|.*...|*....", 
            "row7": ".....|.....|.....|....*|...**|..**.|.**..|**...|*....|.....", 
            "row8": ".....|.....|.....|.....|.....|.....|.....|.....|.....|....."
        }, 
        "speed": 100, 
        "type": "icon"
    }, 
    "widget_key_down": {
        "expression": "return lcd.Transition(-1)", 
        "key": 2, 
        "type": "key"
    }, 
    "widget_key_up": {
        "expression": "return lcd.Transition(1)", 
        "key": 1, 
        "type": "key"
    }, 
    "widget_percent": {
        "expression": "return '%'", 
        "type": "text"
    }, 
    "widget_ram_active": {
        "align": "L", 
        "expression": "return meminfo.Meminfo('Active')/1024", 
        "type": "text", 
        "postfix": "'/'",
        "precision": 0,
        "update": 500, 
        "length": 5
    }, 
    "widget_ram_label": {
        "align": "L", 
        "expression": "return 'Ram:'", 
        "type": "text", 
        "length": 4
    }, 
    "widget_ram_total": {
        "align": "L", 
        "expression": "return meminfo.Meminfo('MemTotal')/1024", 
        "type": "text", 
        "precision": 0,
        "update": 500, 
        "length": 5
    }, 
    "widget_system_bar": {
        "expression": "return procstat.Cpu('system', 500)", 
        "style": "H", 
        "type": "bar", 
        "width": 10
    }, 
    "widget_system_histogram": {
        "direction": "E", 
        "expressionn": "return procstat.Cpu('system', 500)", 
        "gap": 1, 
        "type": "histogram", 
        "width": 10
    }, 
    "widget_system_label": {
        "align": "R", 
        "prefix": "return 'System: '", 
        "type": "text", 
        "length": 10
    }, 
    "widget_wlan0": {
        "align": "L", 
        "expression": "return netdev.Fast('wlan0', 'Tx_bytes', 100) /1024", 
        "precision": 0, 
        "type": "text", 
        "update": 500, 
        "length": 3
    }, 
    "widget_wlan0_bar": {
        "expression":  "return netdev.Fast('wlan0', 'Rx_bytes', 500) / 1024", 
        "expression2": "return netdev.Fast('wlan0', 'Tx_bytes', 500) / 1024", 
        //"style": "H", 
        "type": "bar", 
        "update": 500, 
        "width": 10
    }, 
    "widget_wlan0_histogram": {
        "direction": "E", 
        "expression": 
"return (netdev.Fast('wlan0', 'Rx_bytes', 500) + netdev.Fast('wlan0', 'Tx_bytes', 500)) /1024", 
        "gap": 1, 
        "type": "histogram", 
        "update": 500, 
        "width": 10
    }, 
    "widget_wlan0_label": {
        "expression": "return 'wlan0:'", 
        "type": "text", 
        "length": 6
    }, 
    "widget_wlan0_short_bar": {
        "expression": "return (netdev.Fast('wlan0', 'Rx_bytes', 500) + netdev.Fast('wlan0', 'Tx_bytes', 500)) /1024", 
        "length": 7, 
        "style": "N", 
        "type": "bar", 
        "update": 500
    },
    "widget_netstat_script": {
        "file": "netstat_script.js",
        "type": "script"
    }
}
