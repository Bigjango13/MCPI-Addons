import setuptools

with open("README.md", "r", encoding="utf-8") as f:
    long_description = f.read()

setuptools.setup(
    name="MCPI-Addons",
    version="1.0.0",
    author="Bigjango13",
    description="An extention of the Minecraft Pi API.",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/bigjango13/",
    packages=["mcpi_addons"],
    classifiers=[
        "Programming Language :: Python",
        "Programming Language :: C++",
        "Intended Audience :: Education",
    ],
    # install_requires=[],
    python_requires=">=3.5",
)
