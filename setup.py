import setuptools

with open("README.md", "r", encoding="utf-8") as f:
    long_description = f.read()

setuptools.setup(
    name="MCPI-Addons",
    version="1.2.4",
    author="Bigjango13",
    description="An extension of the Minecraft Pi API.",
    long_description=long_description,
    long_description_content_type="text/markdown",
    url="https://github.com/Bigjango13/MCPI-Addons",
    packages=["mcpi_addons"],
    classifiers=[
        "Programming Language :: Python",
        "Programming Language :: C++",
        "Intended Audience :: Education",
    ],
    python_requires=">=3.8",
)
