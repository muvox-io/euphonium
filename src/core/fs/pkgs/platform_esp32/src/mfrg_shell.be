

import json

# Manufacturing serial shell
class MfrgShell
  def init()
    self.out({
      "event": "ready"
    })
    events.register_native("manufacuring_shell", def (event)
      self.handle_event(event)
    end)
  end
  def handle_event(event)
    data = json.load(event["body"])
    self.out({
      "event": "echo",
      "data": data,
      "data_str": event["body"],
    })
    if data["cmd"] == "i2cscan"
      if i2c.is_installed()
        i2c.delete()
        self.out({
          "event": "i2c_delete_ok",
        })
      end
      i2c.install(data["sda"], data["scl"])
      self.out({
        "event": "i2c_install_ok",
      })
      for i : 0..127
        self.out({
          "event": "i2c_scan",
          "addr": i,
          "status": i2c.detect(i),
        })
      end
      self.out({
        "event": "i2c_scan_done",
      })
    end
    
  end
  def out(data)
    print("MFRG: " + json.dump(data))
  end
end

var mfrgShell = MfrgShell()
