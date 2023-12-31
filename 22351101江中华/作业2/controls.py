from euclid import Vector3
from pyglet import window
from pyglet.event import EVENT_HANDLED
from pyglet.window import key

class Controls:
    """
    Class managing the controls of the application
    """

    def __init__(self, window, camera, bodies, callbacks=None, mouse_sensitivity=0.0025):
        self.window = window
        self.camera = camera
        self.bodies = bodies
        self.callbacks = callbacks
        if not self.callbacks:
            self.callbacks = {}
        self.mouse_sensitivity = mouse_sensitivity

        self.keys = key.KeyStateHandler()
        self.dx = 0
        self.dy = 0

        self.mouse_locked = False
        self.time_multiplier = 1.0
        self.time_multiplier_before_pause = 1.0
        self.paused = False
        self.draw_help_label = True
        self.toggled_help_label = False
        self.selected_body = None
        self.camera_offset = Vector3()

        window.push_handlers(self.on_mouse_press, self.on_mouse_motion, self.on_key_press, self.on_key_release)

    def update(self, delta):
        """
        updates the camera position

        :param delta: delta camera position
        """
        movementspeed = 30 * delta  #

        dx = self.get_dx()
        dy = self.get_dy()

        self.camera.yaw(-1 * dx * self.mouse_sensitivity)
        self.camera.pitch(dy * self.mouse_sensitivity)

        if self.mouse_locked:
            if self.keys[key.LSHIFT]:
                movementspeed *= 10
            if self.keys[key.W]:
                self.camera.forwards(movementspeed)
            if self.keys[key.S]:
                self.camera.backwards(movementspeed)
            if self.keys[key.A]:
                self.camera.left(movementspeed)
            if self.keys[key.D]:
                self.camera.right(movementspeed)
            if self.keys[key.SPACE]:
                self.camera.up(movementspeed)
            if self.keys[key.LCTRL]:
                self.camera.down(movementspeed)

        if self.selected_body:
            self.camera.position += self.selected_body.xyz - self.camera.position - self.camera_offset

    def on_key_press(self, symbol, modifiers):
        """
        Keypress handler

        :param symbol: pressed symbol
        """
        self.keys[symbol] = True
        if symbol == key.ESCAPE:
            if self.selected_body:
                self.selected_body = None
                return EVENT_HANDLED
            else:
                self.window.set_exclusive_mouse(False)
                self.mouse_locked = False
                cb = self.callbacks['toggle_fullscreen']
                if cb is not None:
                    cb(False)
                return EVENT_HANDLED
        # Key code 43: Plus key
        if symbol == key.NUM_ADD or symbol == 43:
            if self.paused:
                self.time_multiplier = self.time_multiplier_before_pause
                self.paused = False
            else:
                if self.keys[key.LSHIFT]:
                    self.time_multiplier += 5.0
                else:
                    self.time_multiplier += 0.1
            return EVENT_HANDLED
        # Key code 45: Minus key
        if symbol == key.NUM_SUBTRACT or symbol == 45:
            if self.paused:
                self.time_multiplier = self.time_multiplier_before_pause
                self.paused = False
            else:
                if self.keys[key.LSHIFT]:
                    self.time_multiplier -= 5.0
                else:
                    self.time_multiplier -= 0.1
            return EVENT_HANDLED
        if symbol == key.P:
            if self.paused:
                self.time_multiplier = self.time_multiplier_before_pause
            else:
                self.time_multiplier_before_pause = self.time_multiplier
                self.time_multiplier = 0
            self.paused = not self.paused
            return EVENT_HANDLED
        if symbol == key.O:
            cb = self.callbacks['toggle_draw_orbits']
            if cb is not None:
                cb()
            return EVENT_HANDLED
        if symbol == key.T:
            cb = self.callbacks['toggle_draw_textures']
            if cb is not None:
                cb()
            return EVENT_HANDLED
        if symbol == key.K:
            cb = self.callbacks['toggle_light']
            if cb is not None:
                cb()
            return EVENT_HANDLED
        if symbol == key.F1:
            self.toggled_help_label = True
            self.draw_help_label = not self.draw_help_label
            return EVENT_HANDLED
        if symbol == key.F11:
            cb = self.callbacks['toggle_fullscreen']
            if cb is not None:
                cb(None)
            return EVENT_HANDLED

    def on_key_release(self, symbol, modifiers):
        """
        called on released key

        :param symbol: released key
        """

        self.keys[symbol] = False

    def on_mouse_press(self, x, y, button, modifiers):
        """
        called on mouse press

        :param x: x location of cursor
        :param y: y location of cursor
        :param button: pressed mouse button
        """

        if button == window.mouse.LEFT:
            if not self.mouse_locked:
                self.window.set_exclusive_mouse(True)
                self.mouse_locked = True
                return EVENT_HANDLED
            else:
                ray = self.camera.create_ray()
                for body in self.bodies:
                    if body.intersects(ray):
                        print(body.name + " hit!")
                        self.selected_body = body
                        self.camera_offset = self.selected_body.xyz - self.camera.position
                        break
                else:
                    self.selected_body = None

    def get_dx(self):
        """
        gets delta x and resets it to 0

        :return: delta x
        """

        tmp = self.dx
        self.dx = 0
        return tmp

    def get_dy(self):
        """
        gets delta y and resets it to 0

        :return: delta y
        """

        tmp = self.dy
        self.dy = 0
        return tmp

    def on_mouse_motion(self, x, y, dx, dy):
        """
        called on mouse movement

        :param x: x location
        :param y: y location
        :param dx: delta x
        :param dy: delta y
        """

        if self.mouse_locked:
            self.dx += dx
            self.dy += dy
            return EVENT_HANDLED
