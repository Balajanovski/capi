import os

from capi.test_common.run_test_command import run_command


def main():
    print(f"Cwd: {os.getcwd()}")
    run_command(["black", "."])
    run_command(["isort", "."])


if __name__ == "__main__":
    main()
