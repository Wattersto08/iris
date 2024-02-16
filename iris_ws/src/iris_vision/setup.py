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
            'talker = iris_vision.ExamplePub:main',
            'listener = iris_vision.ExampleSub:main',            
            'DSLR_handler = iris_vision.DSLR_handler:main',
            'FaceTracking = iris_vision.FaceTracking:main',
            'FaceTracking_headless = iris_vision.FaceTracking_headless:main',
            'facetracking_motioncoltrol = iris_vision.facetracking_motioncontrol:main',
            'iris_joy_control = iris_vision.iris_joy_control:main',
            'Aruco_Tracking = iris_vision.Aruco_Tracking:main',
        ]
    },
)
