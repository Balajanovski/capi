import os

from capi.test_common.run_test_command import run_command


def main():
    print(f"Cwd: {os.getcwd()}")

    options = [
        "--black",
        "--isort",
        "--pylint",
        "--mypy",
        "--cov=capi",
        "--junitxml=python-test-out.xml",
        "--pyargs",
    ]
    run_command(["python3", "-m", "pytest", *options, "capi.test"])


if __name__ == "__main__":
    main()
