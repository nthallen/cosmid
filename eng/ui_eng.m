function ui_eng(dirfunc, stream)
% ui_eng
% ui_eng(dirfunc [, stream])
% dirfunc is a string specifying the name of a function
%   that specifies where data run directories are stored.
% stream is an optional argument specifying which stream
%   the run directories have recorded, e.g. 'SerIn'
if nargin < 1
  dirfunc = 'C:/home/Exp';
end
if nargin >= 2
  f = ne_dialg(stream, 1);
else
  f = ne_dialg('Engineering Data',1);
end
f = ne_dialg(f, 'add', 0, 1, 'gengtm', 'T Mbase' );
f = ne_dialg(f, 'add', 1, 0, 'pengtmtd', 'T Drift' );
f = ne_dialg(f, 'add', 1, 0, 'pengtmcpu', 'CPU' );
f = ne_dialg(f, 'add', 1, 0, 'pengtmram', 'RAM' );
f = ne_dialg(f, 'add', 1, 0, 'pengtmd', 'Disk' );
f = ne_dialg(f, 'add', 0, 1, 'gengpops', 'POPS' );
f = ne_dialg(f, 'add', 1, 0, 'pengpopsd', 'Drift' );
f = ne_dialg(f, 'add', 1, 0, 'pengpopspn', 'Part Num' );
f = ne_dialg(f, 'add', 1, 0, 'pengpopsnum_cc', 'num cc' );
f = ne_dialg(f, 'add', 1, 0, 'pengpopss', 'Surface' );
f = ne_dialg(f, 'add', 1, 0, 'pengpopsv', 'Volume' );
f = ne_dialg(f, 'add', 1, 0, 'pengpopsp', 'P' );
f = ne_dialg(f, 'add', 1, 0, 'pengpopsf', 'Flow' );
f = ne_dialg(f, 'add', 1, 0, 'pengpopsr', 'Raw ccs' );
f = ne_dialg(f, 'add', 1, 0, 'pengpopst', 'T' );
f = ne_dialg(f, 'add', 1, 0, 'pengpopsb', 'Bins' );
f = ne_dialg(f, 'newcol');
f = ne_dialg(f, 'add', 0, 1, 'gengpopshk', 'POPS HK' );
f = ne_dialg(f, 'add', 1, 0, 'pengpopshkstd', 'STD' );
f = ne_dialg(f, 'add', 1, 0, 'pengpopshkldt', 'LD Temp' );
f = ne_dialg(f, 'add', 1, 0, 'pengpopshkldm', 'LD Mon' );
f = ne_dialg(f, 'add', 1, 0, 'pengpopshkb', 'Baseline' );
f = ne_dialg(f, 'add', 1, 0, 'pengpopshks', 'Srvr' );
f = ne_dialg(f, 'add', 1, 0, 'pengpopshkstale', 'Stale' );
f = ne_listdirs(f, dirfunc, 15);
f = ne_dialg(f, 'newcol');
ne_dialg(f, 'resize');
