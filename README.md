# Human Running Animation

Implementation of a human running animation using forward kinematics on Open GL ES.

## Objective

### Problem
* Repeat the given animation every 4 seconds
* Use quaternions to interpolate rotations properly.
* Apply the skinning with the weight blending.
* Character should move only when the screen is being touched. 

### Given Data

All files below are inside app/src/main/cpp/inc/binary/

* skeleton.h: Contains data of 28 joints of the skeleton
    - `jNames`: Array of the name of the joints
    - `jParents`: Array of the index of the parent of each joint
    - `jOffset`: Array of the offset between the joint and its parent joint
* animation.h: Contains animation data with 4 key frames, 0 to 3
    - Each frame consists of 87(= 6 * 1 + 3 * 27) numbers
    - First 6 numbers: XYZ translation, XYZ rotation of the root
    - Every 3 numbers after: XYZ rotation of each joint
    - The rotation is YXZ, i.e. $\hat{v}=R_ZR_XR_Yv$
* player.h: Contains mesh data
    - `playerSize`: The resolution of `playerTexels`
    - `playerTexels`: Array of the square texture
    - `playerVertices`: Array of the mesh
        + Each vertex contains `pos`, `nor`, `tex`, `bone` and `weight`
    - `playerIndices`: Array of the index of the mesh

## Description

Implemented on app/src/main/cpp/src/scene.cpp

### Obtaining Skeletons
* Accumulate the global variable `elapsedTime` by `deltaTime` if `mouseDown` is `true`. `mouseDown` is updated by `mouseUpEvents` and `mouseDownEvents`.
* Since each animation should be 4 seconds and there are 4 keyframes, time interval beetween keyframes is 1 second.
* Therefore, obtain `prevTime`, the latest keyframe time, and `timeDelta`, time difference between `elapsedTime` and `prevTime`.
* Obtain `prevMotion` and `nextMotion` from `motions`.

### Updating Skeletons
* `animations` is a vector of transformation matrices for each joint
* `boneToWorld` is a vector of translation matrices for each joint to the world space
* For each joint,
    - Get Euler angle rotation matrix from `prevMotion` and `nextMotion` array by using `slice` function, then convert to the quaternion matrix by using `getRotationQuat`.
    - Get `rotateMtx` by interpolating quaternion matrices of previous and next motion using `slerp` function.
    - If the joint is root, push the matrix that applies rotation then the translation into `animations`.
    - Else, push the matrix that applies rotation, then translation to its parent, then the animation of its parent into `animations`. Update `boneToWorld` as well.

### Applying Mesh
* `vertices` is a vector of mesh vertices of the frame
* For each given mesh vertex `vertex`,
    - For each bone in `vertex.bone` and its weight in `vertex.weight`, accumulate to `skinningMtx` by the product of root to the bone matrix, animation matrix and the weight of the bone.
    - Update `pos` and `nor` vector of the vertex by `skinningMtx`, then push it to the `vertices` vector

## Environment Setup

* Install following platform and tools in Android Studio
    - Android 11 (API Level 30)
    - Tools to use C++ native language: CMake, NDK v19.2.5345600
* Open ./local.properties, and edit to
```
ndk.dir=<Current AppData Path>\\Local\\Android\\Sdk\\ndk\\19.2.5345600
sdk.dir=<Current AppData Path>\\Local\\Android\\Sdk
```
* Press: File > Sync Project with Gradle Files

## Result
![forward-kinematics](./images/forward-kinematics.gif)

## License and Disclaimer

* The project was submitted as an assignment in COSE331 Computer Graphics course, and is no longer maintained.
* Redistribution of the source code is allowed, however the following consequences are your responsibility.
