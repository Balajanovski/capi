import setuptools

with open("README.md", "r") as handle:
    long_description = handle.read()
with open("requirements.txt", "r") as handle:
    required = handle.read().splitlines()

setuptools.setup(
    name="capi",
    version="1.0.0",
    author="James Balajan",
    author_email="James.Balajan@wisetechglobal.com",
    description="Python library for performing coast aware path interpolation",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="http://tfs.wtg.zone:8080/tfs/CargoWise/DataScience/_git/capi",
    packages=[package for package in setuptools.find_packages() if package not in {"capi.src.test_common"}],
    install_requires=required,
    python_requires=">=3.6",
    zip_safe=False,
    include_package_data=True,
    classifiers=[
        "Programming Language :: Python :: 3",
        "Operating System :: OS Independent",
    ],
)
