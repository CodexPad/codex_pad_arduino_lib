Import("env")
import os


def merge_bin(source, target, env):
    images = env.Flatten(env.get("FLASH_EXTRA_IMAGES", [])) + [
        "$ESP32_APP_OFFSET",
        "$BUILD_DIR/${PROGNAME}.bin",
    ]

    merged_bin_path = os.getenv("MERGED_BIN_PATH")

    if merged_bin_path is None:
        merged_bin_path = os.path.join(
            env.get("BUILD_DIR"),
            os.path.basename(env.get("PROJECT_DIR")) + ".merged.bin",
        )

    cmd = " ".join(
        [
            "$PYTHONEXE",
            "$OBJCOPY",
            "--chip",
            "$BOARD_MCU",
            "merge_bin",
            "--output",
            merged_bin_path,
        ]
        + images
    )
    return env.Execute(cmd)


env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", merge_bin)
