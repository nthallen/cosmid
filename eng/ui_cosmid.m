function ui_cosmid(dirfunc, stream)
% ui_cosmid
% ui_cosmid(dirfunc [, stream])
% dirfunc is a string specifying the name of a function
%   that specifies where data run directories are stored.
% stream is an optional argument specifying which stream
%   the run directories have recorded, e.g. 'SerIn'
if nargin < 1
  dirfunc = 'COSMID_Data_Dir';
end
if nargin >= 2
  f = ne_dialg(stream, 1);
else
  f = ne_dialg('COSMID Instrument',1);
end
f = ne_dialg(f, 'add', 0, 1, 'gcosmidtm', 'T Mbase' );
f = ne_dialg(f, 'add', 1, 0, 'pcosmidtmtd', 'T Drift' );
f = ne_dialg(f, 'add', 1, 0, 'pcosmidtmcpu', 'CPU' );
f = ne_dialg(f, 'add', 1, 0, 'pcosmidtmram', 'RAM' );
f = ne_dialg(f, 'add', 1, 0, 'pcosmidtmd', 'Disk' );
f = ne_dialg(f, 'add', 1, 0, 'pcosmidtmct', 'Core T' );
f = ne_dialg(f, 'add', 0, 1, 'gcosmidpops', 'POPS' );
f = ne_dialg(f, 'add', 1, 0, 'pcosmidpopsd', 'Drift' );
f = ne_dialg(f, 'add', 1, 0, 'pcosmidpopspn', 'Part Num' );
f = ne_dialg(f, 'add', 1, 0, 'pcosmidpopsnum_cc', 'num cc' );
f = ne_dialg(f, 'add', 1, 0, 'pcosmidpopss', 'Surface' );
f = ne_dialg(f, 'add', 1, 0, 'pcosmidpopsv', 'Volume' );
f = ne_dialg(f, 'add', 1, 0, 'pcosmidpopsp', 'P' );
f = ne_dialg(f, 'add', 1, 0, 'pcosmidpopsf', 'Flow' );
f = ne_dialg(f, 'add', 1, 0, 'pcosmidpopsr', 'Raw ccs' );
f = ne_dialg(f, 'add', 1, 0, 'pcosmidpopst', 'T' );
f = ne_dialg(f, 'add', 1, 0, 'pcosmidpopsb', 'Bins' );
f = ne_dialg(f, 'newcol');
f = ne_dialg(f, 'add', 0, 1, 'gcosmidpopshk', 'POPS HK' );
f = ne_dialg(f, 'add', 1, 0, 'pcosmidpopshkstd', 'STD' );
f = ne_dialg(f, 'add', 1, 0, 'pcosmidpopshkldt', 'LD Temp' );
f = ne_dialg(f, 'add', 1, 0, 'pcosmidpopshkldm', 'LD Mon' );
f = ne_dialg(f, 'add', 1, 0, 'pcosmidpopshkb', 'Baseline' );
f = ne_dialg(f, 'add', 1, 0, 'pcosmidpopshks', 'Srvr' );
f = ne_dialg(f, 'add', 1, 0, 'pcosmidpopshkstale', 'Stale' );
f = ne_listdirs(f, dirfunc, 15);
f = ne_dialg(f, 'newcol');
ne_dialg(f, 'resize');
