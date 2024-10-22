# coretemps module
This module depends on the sensors library, which is
Linux-specific. The library is only required for collection,
but by default, all programs are built on all platforms.
To work around this, the coretemps module itself has two
modes, default and no_sensors. That works OK, but it requires
the cosmid.spec be modified on the GSE to select the no_sensors
mode, and that is error-prone. It is easy for the modified
.spec file to get accidentally committed and propagated to
the flight computer, which would disable the collection of
the coretemps.

The solution is to add another level of indirection:

  - cosmid.spec invokes Module coretemps_select
  - coretemps_select.agm is a soft link to one of:
    - coretemps_select.agm.sensors
    - coretemps_select.agm.no_sensors

To enable sensors:

```$ ln -s coretemps_select.agm.sensors coretemps_select```

To disable sensors:

```$ ln -s coretemps_select.agm.no_sensors coretemps_select```

The current disadvantage of this approach is that it does not
build out of the box. I would like an approach that defaults to
enabling sensors but where the no_sensors approach can be selected
by adding a file.
