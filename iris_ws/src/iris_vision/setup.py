from setuptools import find_packages, setup

package_name = 'iris_vision'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='Tom Watters',
    maintainer_email='tom.j.watters@gmail.com',
    description='TODO: Package description',
    license='TODO: License declaration',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            #'testsub = iris_vision.image_sub:main',
            'talker = iris_vision.publisher_member_function:main',
            'listener = iris_vision.subscriber_member_function:main',
        ]
    },
)
