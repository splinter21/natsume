from setuptools import Extension, find_packages, setup

setup(
    name="natsume",
    version="0.0.1",
    description="A Japanese text frontend processing toolkit",
    author="Francis Hu",
    author_email="franciskomizu@gmail.com",
    url="https://github.com/Francis-Komizu/natsume",
    license="GNU GPL License",
    packages=find_packages(),
    platforms="any",
    install_requires=[
        "numpy >= 1.20.0",
    ]
)