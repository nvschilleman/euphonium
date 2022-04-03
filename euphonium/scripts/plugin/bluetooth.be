
class BluetoothPlugin : Plugin

    def init()
        self.apply_default_values()
        self.theme_color = "#287AA9"
        self.name = "bluetooth"
        self.display_name = "Bluetooth"
        self.type = "plugin"
    end

    def make_form(ctx, state)
        ctx.create_group('bluetooth', { 'label': 'General' })

        ctx.text_field('name', {
            'label': "Device's name",
            'default': "XD",
            'group': 'bluetooth'
        })
    end
end

euphonium.register_plugin(BluetoothPlugin())
