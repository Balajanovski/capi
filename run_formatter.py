import os

from capi.test_common.run_test_command import run_command


def main():
    print(f"Cwd: {os.getcwd()}")
    run_command(["python3", "-m", "black", "capi"])
    run_command(["python3", "-m", "isort", "capi"])


if __name__ == "__main__":
    main()
