# 安装依赖

```sh
sh dev_setup.sh
```

# 运行

```sh
python game.py
```

### 报错

> RuntimeError: generator rasised StopIteration

python 版本原因，找到报错地方，对 next() 进行异常处理，例如：

```python
try:
	next_value = next(iterator)
except StopIteration:
    return
```

# 运行结果

```
w、s、a、d：前后左右移动视角
空格、ctrl：上下移动视角
p：暂停转动
T：纹理贴图开关
K：光源开关
Esc：解锁鼠标/退出天体跟踪
```



点击左边按钮，可以跳到对应天体并跟踪



### **天体分布**

Sun 太阳

- Mercury 水星
- Venus 金星
- Earth 地球
  - Moon 月球
- Mars 火星
- Jupiter 木星
  - Io 木卫一
  - Europa 木卫二
- Saturn 土星
  - Enceladus 土卫二
  - Dione 土卫四
  - Iapetus 土卫八
- Uranus 天王星
- Neptune 海王星
- Pluto 冥王星