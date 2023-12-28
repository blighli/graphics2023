// 创建场景
var scene = new THREE.Scene();

// 创建摄像机
var camera = new THREE.PerspectiveCamera(75, window.innerWidth / window.innerHeight, 0.1, 1000);
camera.position.z = 20;

// 创建 WebGL 渲染器
var renderer = new THREE.WebGLRenderer();
renderer.setSize(window.innerWidth, window.innerHeight);
document.body.appendChild(renderer.domElement);

// 创建纹理加载器
var textureLoader = new THREE.TextureLoader();

// 创建点光源，模拟太阳光
var light = new THREE.PointLight(0xffffff, 1.5, 1000);
light.position.set(0, 0, 0); // 设置光源位置为太阳位置
scene.add(light);

// 创建太阳
var sunTexture = textureLoader.load('textures/8k_sun.jpg');
var sunGeometry = new THREE.SphereGeometry(3, 32, 32);
var sunMaterial = new THREE.MeshBasicMaterial({ map: sunTexture }); // 使用MeshBasicMaterial，使太阳不受光照影响
var sun = new THREE.Mesh(sunGeometry, sunMaterial);
sun.name = 'Sun';
scene.add(sun);

// 创建银河背景
var starTexture = textureLoader.load('textures/8k_stars_milky_way.jpg');
var skyGeometry = new THREE.SphereGeometry(500, 64, 64);
var skyMaterial = new THREE.MeshBasicMaterial({
    map: starTexture,
    side: THREE.BackSide
});
var skySphere = new THREE.Mesh(skyGeometry, skyMaterial);
scene.add(skySphere);

// 创建其他行星
function createPlanet(size, texture_path, distanceFromSun, orbitSpeed, name) {
    var planetTexture = textureLoader.load(texture_path);
    var planetGeometry = new THREE.SphereGeometry(size, 32, 32);
    var planetMaterial = new THREE.MeshPhongMaterial({ map: planetTexture }); // 使用MeshPhongMaterial或MeshLambertMaterial
    var planet = new THREE.Mesh(planetGeometry, planetMaterial);
    planet.name = name;
    planet.position.set(distanceFromSun, 0, 0);
    scene.add(planet);
    return { mesh: planet, orbitSpeed: orbitSpeed, distanceFromSun: distanceFromSun };
}
var planets = [
    createPlanet(0.38, 'textures/8k_mercury.jpg',       10, 6.4, 'Mercury'), // 水星
    createPlanet(0.95, 'textures/8k_venus_surface.jpg', 15, 4.8, 'Venus'),   // 金星
    createPlanet(1.00, 'textures/8k_earth_nightmap.jpg',20, 3.2, 'Earth'),   // 地球
    createPlanet(0.53, 'textures/8k_mars.jpg',          25, 2.4, 'Mars'),    // 火星
    createPlanet(2.00, 'textures/8k_jupiter.jpg',       30, 1.3, 'Jupiter'), // 木星
    createPlanet(1.70, 'textures/8k_saturn.jpg',        35, 1.1, 'Saturn'),  // 土星
    createPlanet(1.50, 'textures/2k_uranus.jpg',        40, 0.9, 'Uranus'),  // 天王星
    createPlanet(1.40, 'textures/2k_neptune.jpg',       45, 0.8, 'Neptune'), // 海王星
];

// 创建射线投射器和鼠标向量
var raycaster = new THREE.Raycaster();
var mouse = new THREE.Vector2();

// 处理鼠标点击事件
function onMouseClick(event) {
    // 防止浏览器的默认行为
    event.preventDefault();

    // 将鼠标位置转换为标准化设备坐标（NDC）
    mouse.x = (event.clientX / renderer.domElement.clientWidth) * 2 - 1;
    mouse.y = -(event.clientY / renderer.domElement.clientHeight) * 2 + 1;
    
    // 更新射线投射器的射线
    raycaster.setFromCamera(mouse, camera);

    // 检查射线与哪些对象相交
    var intersects = raycaster.intersectObjects(scene.children, true);

    // 显示行星名称
    if (intersects.length > 0) {
        alert(intersects[0].object.name);
    }
}
window.addEventListener('click', onMouseClick);

// 创建动画效果
var animate = function () {
    requestAnimationFrame(animate);
    //太阳自转
    sun.rotation.y += 0.005;
    // 更新行星位置
    planets.forEach(function(planetInfo) {
        // 行星自转
        planetInfo.mesh.rotation.y += 0.005;
        // 行星公转
        var angle = Date.now() * 0.0001 * planetInfo.orbitSpeed;
        planetInfo.mesh.position.x = planetInfo.distanceFromSun * Math.cos(angle);
        planetInfo.mesh.position.z = planetInfo.distanceFromSun * Math.sin(angle);
    });

    renderer.render(scene, camera);
};

animate();
