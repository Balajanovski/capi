import os
import subprocess
import typing


def run_command(command: typing.List[str], env: typing.Optional[typing.Dict[str, str]] = None):
    joined_command = " ".join(command)
    working_env = os.environ.copy()

    if env is None:
        print(f"> {joined_command}")
        subprocess.run(command, check=True, env=working_env)
    else:
        for name, value in env.items():
            working_env[name] = value
        print(f"> {joined_command}  {env}")
        subprocess.run(command, check=True, env=working_env)
