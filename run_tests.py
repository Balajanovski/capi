import os

from capi.test_common.run_test_command import run_command


def main():
    print(f"Cwd: {os.getcwd()}")

    options = [
        "--black",
        "--isort",
        "--pylint",
        "--mypy",
        "--cov",
        "--junitxml=python-test-out.xml",
    ]
    run_command(["python3", "-m", "pytest", *options, "capi/test"])


if __name__ == "__main__":
    main()
