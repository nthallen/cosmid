function fig = gengpops(varargin);
% gengpops(...)
% POPS
ffig = ne_group(varargin,'POPS','pengpopsd','pengpopspn','pengpopsnum_cc','pengpopss','pengpopsv','pengpopsp','pengpopsf','pengpopsr','pengpopst','pengpopsb');
if nargout > 0 fig = ffig; end
