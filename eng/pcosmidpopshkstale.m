function pcosmidpopshkstale(varargin);
% pcosmidpopshkstale( [...] );
% POPS HK Stale
h = timeplot({'POPS_Stale','POPS_DrvStale'}, ...
      'POPS HK Stale', ...
      'Stale', ...
      {'POPS\_Stale','POPS\_DrvStale'}, ...
      varargin{:} );