from setuptools import setup, Extension

module = Extension('kmeans_module', sources=['kmeans_module.c'])

setup(
    name='kmeans_module',
    version='1.0',
    description='C extension for kmeans programs',
    ext_modules=[module]
)
