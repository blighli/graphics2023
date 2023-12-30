let scene, camera, renderer, model;
let rotate = false, zoom = false, pan = false, mtlFile = null;

// 初始化场景、相机和渲染器
init();
animate();

function init() {
    // 初始化场景和相机
    scene = new THREE.Scene();
    camera = new THREE.PerspectiveCamera(75, window.innerWidth / window.innerHeight, 0.1, 1000);
    camera.position.z = 300;
    camera.position.y = 100;

    renderer = new THREE.WebGLRenderer({canvas: document.getElementById('webgl-canvas')});
    renderer.setSize(window.innerWidth, window.innerHeight);

    // 加载3d模型例子
    loadModel('model-example/IronMan.mtl', 'model-example/IronMan.obj');

    // 添加多个光源
    // 环境光
    const ambientLight = new THREE.AmbientLight(0x404040, 1.0);
    scene.add(ambientLight);

    // 定向光
    const directionalLight = new THREE.DirectionalLight(0xffffff, 0.5);
    directionalLight.position.set(1, 1, 1);
    scene.add(directionalLight);

    // 点光源
    const pointLight1 = new THREE.PointLight(0xff0000, 1, 100);
    pointLight1.position.set(50, 50, 50);
    scene.add(pointLight1);

    const pointLight2 = new THREE.PointLight(0x0000ff, 1, 100);
    pointLight2.position.set(-50, -50, -50);
    scene.add(pointLight2);

    document.getElementById('mtlInput').addEventListener('change', function(event) {
        const file = event.target.files[0];
        if (!file) return;
        const reader = new FileReader();
        reader.onload = function(e) {
            mtlFile = e.target.result;
        };
        reader.readAsDataURL(file);
    });

    // document.getElementById('modelInput').addEventListener('change', function(event) {
    //     const file = event.target.files[0];
    //     if (!file) return;
    //     const reader = new FileReader();
    //     reader.onload = function(e) {
    //         const objContents = e.target.result;
    //         if (mtlFile) {
    //             // 如果有MTL文件，使用MTLLoader
    //             const mtlLoader = new THREE.MTLLoader();
    //             mtlLoader.load(mtlFile, function (materials) {
    //                 materials.preload();
    //                 loadObjWithMaterials(objContents, materials);
    //             });
    //         } else {
    //             // 如果没有MTL文件，直接加载OBJ文件
    //             loadObj(objContents);
    //         }
    //     };
    //     reader.readAsDataURL(file);
    // });

    document.getElementById('modelInput').addEventListener('change', function(event) {
        const mtlFileInput = document.getElementById('mtlInput');
        if (mtlFileInput.files.length > 0 && event.target.files.length > 0) {
            const mtlFile = mtlFileInput.files[0];
            const objFile = event.target.files[0];
    
            const mtlReader = new FileReader();
            mtlReader.onload = function(e) {
                const objReader = new FileReader();
                objReader.onload = function(e) {
                    loadModel(mtlReader.result, objReader.result);
                };
                objReader.readAsDataURL(objFile);
            };
            mtlReader.readAsDataURL(mtlFile);
        }
    });

    // 添加鼠标事件监听器
    document.addEventListener('mousemove', onMouseMove, false);
    document.addEventListener('wheel', onWheel, false);

    // 添加键盘事件监听
    document.addEventListener('keydown', onKeyDown, false);
}

function loadModel(mtlPath, objPath) {
    const mtlLoader = new THREE.MTLLoader();
    mtlLoader.load(mtlPath, function (materials) {
        materials.preload();
        const objLoader = new THREE.OBJLoader();
        objLoader.setMaterials(materials);
        objLoader.load(objPath, function (object) {
            // 移除之前的模型（如果有的话）
            if (model) {
                scene.remove(model);
            }
            model = object;
            scene.add(model);
        });
    });
}

function loadObjWithMaterials(objContents, materials) {
    const objLoader = new THREE.OBJLoader();
    objLoader.setMaterials(materials);
    objLoader.load(objContents, function (object) {
        scene.add(object);
        model = object;
    });
}

function loadObj(objContents) {
    const objLoader = new THREE.OBJLoader();
    objLoader.load(objContents, function (object) {
        scene.add(object);
        model = object;
    });
}

function animate() {
    requestAnimationFrame(animate);
    renderer.render(scene, camera);
}

// 处理鼠标移动事件
function onMouseMove(event) {
    if (!model) return;
    // 平移
    if (event.buttons === 1) {
        model.position.x += (event.movementX / 100);
        model.position.y -= (event.movementY / 100);
    }
    // 旋转
    if (event.buttons === 4) {
        model.rotation.y += event.movementX * 0.01;
        model.rotation.x += event.movementY * 0.01;
    }
}

// 处理鼠标滚轮事件
function onWheel(event) {
    if (!model) return;
    // 缩放
    let scaleFactor = 1 + event.deltaY * -0.001;
    scaleFactor = Math.min(Math.max(.125, scaleFactor), 4);
    model.scale.x *= scaleFactor;
    model.scale.y *= scaleFactor;
    model.scale.z *= scaleFactor;
}

// 处理键盘事件
function onKeyDown(event) {
    switch(event.keyCode) {
        case 87: // W键
            moveForward(0.5);
            break;
        case 83: // S键
            moveBackward(0.5);
            break;
        case 65: // A键
            moveLeft(0.5);
            break;
        case 68: // D键
            moveRight(0.5);
            break;
        case 37: // 左箭头
            rotateLeft(0.1);
            break;
        case 39: // 右箭头
            rotateRight(0.1);
            break;
    }
}

// 相机移动和旋转函数
function moveForward(distance) {
    camera.translateZ(-distance);
}

function moveBackward(distance) {
    camera.translateZ(distance);
}

function moveLeft(distance) {
    camera.translateX(-distance);
}

function moveRight(distance) {
    camera.translateX(distance);
}

function rotateLeft(angle) {
    camera.rotateY(angle);
}

function rotateRight(angle) {
    camera.rotateY(-angle);
}