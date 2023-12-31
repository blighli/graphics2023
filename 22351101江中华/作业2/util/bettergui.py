from pyglet_gui.controllers import TwoStateController
from pyglet_gui.core import Viewer
from pyglet_gui.gui import pyglet
from pyglet_gui.override import Label


# Some classes copied from pyglet-gui to add some more functionality

class BetterLabel(Viewer):
    def __init__(self, text="", bold=False, italic=False, font_name=None, font_size=None, color=None, multiline=False, lblwidth=None, lblalign='left', path=None):
        super().__init__(self)
        self.text = text
        self.bold = bold
        self.italic = italic
        self.font_name = font_name
        self.font_size = font_size
        self.color = color
        self.multiline = multiline
        self.lblwidth = lblwidth
        self.lblalign = lblalign
        self.path = path
        self.label = None

    def get_path(self):
        return self.path

    def unload_graphics(self):
        self.label.delete()

    def layout(self):
        font = self.label.document.get_font()
        self.label.x = self.x
        self.label.y = self.y - font.descent

    def set_text(self, text):
        self.text = text
        self.reload()
        self.reset_size()

    def compute_size(self):
        font = self.label.document.get_font()
        return self.label.content_width, font.ascent - font.descent

    def load_graphics(self):
        theme = self.theme[self.get_path()]

        self.label = pyglet.text.Label(self.text,
                                       bold=self.bold,
                                       italic=self.italic,
                                       color=self.color or theme['text_color'],
                                       font_name=self.font_name or theme['font'],
                                       font_size=self.font_size or theme['font_size'],
                                       multiline=self.multiline,
                                       width=self.lblwidth or theme['lblwidth'],
                                       align=self.lblalign,
                                       **self.get_batch('background'))


class BetterButton(TwoStateController, Viewer):
    def __init__(self, label="", is_pressed=False, on_press=None, color=None):
        TwoStateController.__init__(self, is_pressed=is_pressed, on_press=on_press)
        Viewer.__init__(self)

        self.color = color
        self.label = label

        # graphics
        self._label = None
        self._button = None

    def change_state(self):
        self._is_pressed = not self._is_pressed
        self.reload()
        self.reset_size()
        self._on_press(self._is_pressed)

    def hit_test(self, x, y):
        return self.is_inside(x, y)

    def on_mouse_press(self, x, y, button, modifiers):
        self.change_state()

    def get_path(self):
        path = ['button']
        if self.is_pressed:
            path.append('down')
        else:
            path.append('up')
        return path

    def load_graphics(self):
        theme = self.theme[self.get_path()]

        self._button = theme['image'].generate(self.color or theme['gui_color'], **self.get_batch('background'))

        self._label = Label(self.label,
                            font_name=theme['font'],
                            font_size=theme['font_size'],
                            color=theme['text_color'],
                            **self.get_batch('foreground'))

    def unload_graphics(self):
        self._button.unload()
        self._label.unload()

    def compute_size(self):
        # Treat the height of the label as ascent + descent
        font = self._label.document.get_font()
        height = font.ascent - font.descent

        return self._button.get_needed_size(self._label.content_width, height)

    def layout(self):
        self._button.update(self.x, self.y, self.width, self.height)

        # centers the label on the middle of the button
        x, y, width, height = self._button.get_content_region()

        font = self._label.document.get_font()
        self._label.x = x + width / 2 - self._label.content_width / 2
        self._label.y = y + height / 2 - font.ascent / 2 - font.descent
        self._label.update()

    def delete(self):
        TwoStateController.delete(self)
        Viewer.delete(self)


class BetterOneTimeButton(BetterButton):
    def __init__(self, label="", on_release=None, color=None):
        BetterButton.__init__(self, label=label, color=color)

        self.on_release = lambda x: x
        if on_release is not None:
            self.on_release = on_release

    def on_mouse_release(self, x, y, button, modifiers):
        if self.is_pressed:
            self.change_state()

            # If mouse is still hovering us, signal on_release
            if self.hit_test(x, y):
                self.on_release(self._is_pressed)
