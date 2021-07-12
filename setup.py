import sys

import setuptools

try:
    from skbuild import setup
except ImportError:
    print(
        "Please update pip, you need pip 10 or greater,\n"
        " or you need to install the PEP 518 requirements in pyproject.toml yourself",
        file=sys.stderr,
    )
    raise


with open("README.md", "r") as handle:
    long_description = handle.read()
with open("requirements.txt", "r") as handle:
    required = handle.read().splitlines()

setup(
    name="capi-vis",
    version="2.3.7",
    author="James Balajan",
    author_email="James.Balajan@wisetechglobal.com",
    description="Python library for performing coast aware path interpolation using visibility graphs",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/Balajanovski/capi",
    packages=[f"capi.src.{package}" for package in setuptools.find_packages(where="capi/src")],
    install_requires=required,
    python_requires=">=3.6",
    zip_safe=False,
    include_package_data=True,
    classifiers=[
        "Programming Language :: Python :: 3",
        "Operating System :: OS Independent",
    ],
    cmake_install_dir="capi/src/implementation/visibility_graphs",
)
