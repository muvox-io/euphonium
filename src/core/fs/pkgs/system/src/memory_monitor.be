class MemoryMonitor
    def member(name)
        return get_native('memory_monitor', name)
    end
end

memory_monitor = MemoryMonitor()


