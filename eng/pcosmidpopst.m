function pcosmidpopst(varargin)
% pcosmidpopst( [...] );
% POPS T
h = timeplot({'POPS_AirTemp','POPS_LDTemp','POPS2_AirTemp','POPS2_LDTemp'}, ...
      'POPS T', ...
      'T', ...
      {'POPS\_AirTemp','POPS\_LDTemp','POPS2\_AirTemp','POPS2\_LDTemp'}, ...
      varargin{:} );
