function pcosmidpopshkt(varargin)
% pcosmidpopshkt( [...] );
% POPS HK Temp
h = timeplot({'POPS_LDTemp','POPS_LD_Mon','POPS2_LDTemp','POPS2_LD_Mon'}, ...
      'POPS HK Temp', ...
      'Temp', ...
      {'POPS\_LDTemp','POPS\_LD\_Mon','POPS2\_LDTemp','POPS2\_LD\_Mon'}, ...
      varargin{:} );