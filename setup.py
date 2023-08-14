from setuptools import Extension
from setuptools import setup

module = Extension(
    "mueler_brown",
    sources=["mueler_brown.c"],
    extra_link_args=["-lgsl", "-lgslcblas", "-lm"],
)
setup(
    name="mueler_brown",
    version="1.0.0",
    description="A Mueler-Brown potential module",
    url="https://github.com:slitvinov/mueler-brown.git",
    author="Sergey Litvinov",
    author_email="slitvinov@gmail.com",
    license="MIT",
    classifiers=[
        "Programming Language :: Python :: 3",
    ],
    ext_modules=[module],
)
