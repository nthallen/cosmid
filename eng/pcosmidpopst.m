function pcosmidpopst(varargin)
% pcosmidpopst( [...] );
% POPS T
h = timeplot({'POPS_AirTemp','POPS2_AirTemp'}, ...
      'POPS T', ...
      'T', ...
      {'POPS\_AirTemp','POPS2\_AirTemp'}, ...
      varargin{:} );
