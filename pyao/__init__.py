"""
Python libao interface.
"""

__all__ = [
    'presets',
    'wave',
    'tone',
    'PlaybackError',
    'AODevice',
    'AOFormat',
    'init',
    'open',
    'shutdown',
    'get_default_driver_id',
    'AO_FMT_NATIVE',
    'AO_FMT_LITTLE',
    'AO_FMT_BIG'
]

from pyao._abstract import (
    PlaybackError,
    AODevice,
    AOFormat,
    pyao_init as init,
    open,
    pyao_shutdown as shutdown,
    pyao_default_driver_id as get_default_driver_id,
    AO_FMT_NATIVE,
    AO_FMT_LITTLE,
    AO_FMT_BIG
)
from pyao import presets, wave, tone
