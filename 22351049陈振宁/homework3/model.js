let scene, camera, renderer, model;
let rotate = false, zoom = false, pan = false, mtlFile = null;

// initialize
init();
animate();

function init() {
    // initialize
    scene = new THREE.Scene();
    camera = new THREE.PerspectiveCamera(75, window.innerWidth / window.innerHeight, 0.1, 1000);
    camera.position.z = 300;
    camera.position.y = 100;

    renderer = new THREE.WebGLRenderer({canvas: document.getElementById('webgl-canvas')});
    renderer.setSize(window.innerWidth, window.innerHeight);

    // load 3d model
    loadModel('dataset/Bird.mtl', 'dataset/Bird.obj');

    // ad some light source
    // environmental light
    const ambientLight = new THREE.AmbientLight(0x404040, 1.0);
    scene.add(ambientLight);

    // fixed-direction light
    const directionalLight = new THREE.DirectionalLight(0xffffff, 0.5);
    directionalLight.position.set(1, 1, 1);
    scene.add(directionalLight);

    // dot light
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

    document.addEventListener('mousemove', onMouseMove, false);
    document.addEventListener('wheel', onWheel, false);

    document.addEventListener('keydown', onKeyDown, false);
}

function loadModel(mtlPath, objPath) {
    const mtlLoader = new THREE.MTLLoader();
    mtlLoader.load(mtlPath, function (materials) {
        materials.preload();
        const objLoader = new THREE.OBJLoader();
        objLoader.setMaterials(materials);
        objLoader.load(objPath, function (object) {
            // remove previous model
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

// handle mouse moving incident
function onMouseMove(event) {
    if (!model) return;
    // pan
    if (event.buttons === 1) {
        model.position.x += (event.movementX / 100);
        model.position.y -= (event.movementY / 100);
    }
    // rotate
    if (event.buttons === 4) {
        model.rotation.y += event.movementX * 0.01;
        model.rotation.x += event.movementY * 0.01;
    }
}

// handle mouse rolling incident
function onWheel(event) {
    if (!model) return;
    let scaleFactor = 1 + event.deltaY * -0.001;
    scaleFactor = Math.min(Math.max(.125, scaleFactor), 4);
    model.scale.x *= scaleFactor;
    model.scale.y *= scaleFactor;
    model.scale.z *= scaleFactor;
}

// handle keyboard incident
function onKeyDown(event) {
    switch(event.keyCode) {
        case 87: // W
            moveForward(0.5);
            break;
        case 83: // S
            moveBackward(0.5);
            break;
        case 65: // A
            moveLeft(0.5);
            break;
        case 68: // D
            moveRight(0.5);
            break;
        case 37: // left arrow
            rotateLeft(0.1);
            break;
        case 39: // right arrow
            rotateRight(0.1);
            break;
    }
}

// camera moving function
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