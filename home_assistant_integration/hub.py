"""A demonstration 'hub' that connects several devices."""
from __future__ import annotations

import asyncio
import random
import aiohttp

from homeassistant.core import HomeAssistant

class FeederDevice:
    """Pet feeder device."""
    
    def __init__(self, hass: HomeAssistant, name: str, url: str) -> None:
        """Init dummy hub."""
        self._url = url
        self._hass = hass
        self._name = name
        self._id = url.lower()
        # self.online = True

        self._callbacks = set()
        self._loop = asyncio.get_event_loop()
        self._target_position = 100
        self._current_position = 100
        # Reports if the roller is moving up or down.
        # >0 is up, <0 is down. This very much just for demonstration.
        self.moving = 0

        # Some static information about this device
        self.firmware_version = None
        self.model = None
        self.manufacturer = None

    @property
    def feeder_id(self) -> str:
        """Hub Id"""
        return self._id

    @property
    def name(self) -> str:
        """Hub Name"""
        return self._name

    @property
    def feeder_url(self) -> str:
        """Hub Host"""
        return self._url

    async def update_info(self) -> bool:
        """Test connectivity to the Dummy hub is OK."""
        async with aiohttp.ClientSession() as session:
            async with session.get(f'{self._url}/deviceInfo') as resp:
                info = await resp.json()
                self.firmware_version = info['version']
                self.model = info['model']
                self.manufacturer = info['manufacturer']
        return True

    @property
    def position(self):
        """Return position for roller."""
        return self._current_position

    async def set_position(self, position: int) -> None:
        """
        Set dummy cover to the given position.

        State is announced a random number of seconds later.
        """
        self._target_position = position

        # Update the moving status, and broadcast the update
        self.moving = position - 50
        await self.publish_updates()

        self._loop.create_task(self.delayed_update())

    async def delayed_update(self) -> None:
        """Publish updates, with a random delay to emulate interaction with device."""
        await asyncio.sleep(random.randint(1, 10))
        self.moving = 0
        await self.publish_updates()

    def register_callback(self, callback: Callable[[], None]) -> None:
        """Register callback, called when Roller changes state."""
        self._callbacks.add(callback)

    def remove_callback(self, callback: Callable[[], None]) -> None:
        """Remove previously registered callback."""
        self._callbacks.discard(callback)

    # In a real implementation, this library would call it's call backs when it was
    # notified of any state changeds for the relevant device.
    async def publish_updates(self) -> None:
        """Schedule call all registered callbacks."""
        self._current_position = self._target_position
        for callback in self._callbacks:
            callback()

    @property
    def online(self) -> float:
        """Roller is online."""
        # The dummy roller is offline about 10% of the time. Returns True if online,
        # False if offline.
        return random.random() > 0.1

    @property
    def battery_level(self) -> int:
        """Battery level as a percentage."""
        return random.randint(0, 100)

    @property
    def battery_voltage(self) -> float:
        """Return a random voltage roughly that of a 12v battery."""
        return round(random.random() * 3 + 10, 2)

    @property
    def illuminance(self) -> int:
        """Return a sample illuminance in lux."""
        return random.randint(0, 500)
