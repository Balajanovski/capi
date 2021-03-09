import os

from capi.src.test_common.run_test_command import run_command


def main():
    print(f"Cwd: {os.getcwd()}")

    options = [
        "--black",
        "--isort",
        "--pylint",
        "--mypy",
    ]
    run_command(["pytest", *options, "capi/test"])


if __name__ == "__main__":
    main()
