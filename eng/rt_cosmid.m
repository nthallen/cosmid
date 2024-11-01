function dfs_out = rt_cosmid(dfs)
% dfs = rt_cosmid()
%   Create a data_fields object and setup all the buttons for realtime
%   plots
% dfs_out = rt_cosmid(dfs)
%   Use the data_fields object and setup all the buttons for realtime plots
if nargin < 1 || isempty(dfs)
  dfs = data_fields('title', 'COSMID Instrument', ...
    'Color', [.8 .8 1], ...
    'h_leading', 8, 'v_leading', 2, ...
    'btn_fontsize', 12, ...
    'txt_fontsize', 12);
  context_level = dfs.rt_init;
else
  context_level = 1;
end
dfs.start_col;
dfs.plot('tm', 'label', 'T Mbase', 'plots', {'tmtd','tmcpu','tmram','tmd','tmct'});
dfs.plot('tmtd','label','T Drift','vars',{'SysTDrift'});
dfs.plot('tmcpu','label','CPU','vars',{'CPU_Pct'});
dfs.plot('tmram','label','RAM','vars',{'memused'});
dfs.plot('tmd','label','Disk','vars',{'Disk'});
dfs.plot('tmct','label','Core T','vars',{'coretemp[4]'});
dfs.plot('pops', 'label', 'POPS', 'plots', {'popsd','popspn','popsnum_cc','popss','popsv','popsp','popsf','popsr','popst','popsb'});
dfs.plot('popsd','label','Drift','vars',{'POPS_Tdrift'});
dfs.plot('popspn','label','Part Num','vars',{'POPS_Part_Num'});
dfs.plot('popsnum_cc','label','num cc','vars',{'POPS_num_cc'});
dfs.plot('popss','label','Surface','vars',{'POPS_Surf_Area'});
dfs.plot('popsv','label','Volume','vars',{'POPS_Vol_Density'});
dfs.plot('popsp','label','P','vars',{'POPS_P_mbar'});
dfs.plot('popsf','label','Flow','vars',{'POPS_ccm'});
dfs.plot('popsr','label','Raw ccs','vars',{'POPS_Flow'});
dfs.plot('popst','label','T','vars',{'POPS_AirTemp'});
dfs.plot('popsb','label','Bins','vars',{'POPS_Bin[16]'});
dfs.end_col;
dfs.start_col;
dfs.plot('popshk', 'label', 'POPS HK', 'plots', {'popshkstd','popshkldt','popshkldm','popshkb','popshks','popshkstale'});
dfs.plot('popshkstd','label','STD','vars',{'POPS_STD'});
dfs.plot('popshkldt','label','LD Temp','vars',{'POPS_LDTemp'});
dfs.plot('popshkldm','label','LD Mon','vars',{'POPS_LD_Mon'});
dfs.plot('popshkb','label','Baseline','vars',{'POPS_Baseline'});
dfs.plot('popshks','label','Srvr','vars',{'POPS_Srvr'});
dfs.plot('popshkstale','label','Stale','vars',{'POPS_Stale','POPS_DrvStale'});
dfs.plot('pops2', 'label', 'POPS2', 'plots', {'pops2d','pops2pn','pops2num_cc','pops2s','pops2v','pops2p','pops2f','pops2r','pops2t','pops2b'});
dfs.plot('pops2d','label','Drift','vars',{'POPS2_Tdrift'});
dfs.plot('pops2pn','label','Part Num','vars',{'POPS2_Part_Num'});
dfs.plot('pops2num_cc','label','num cc','vars',{'POPS2_num_cc'});
dfs.plot('pops2s','label','Surface','vars',{'POPS2_Surf_Area'});
dfs.plot('pops2v','label','Volume','vars',{'POPS2_Vol_Density'});
dfs.plot('pops2p','label','P','vars',{'POPS2_P_mbar'});
dfs.plot('pops2f','label','Flow','vars',{'POPS2_ccm'});
dfs.plot('pops2r','label','Raw ccs','vars',{'POPS2_Flow'});
dfs.plot('pops2t','label','T','vars',{'POPS2_AirTemp'});
dfs.plot('pops2b','label','Bins','vars',{'POPS2_Bin[16]'});
dfs.end_col;
dfs.start_col;
dfs.plot('pops2_hk', 'label', 'POPS2 HK', 'plots', {'pops2_hkstd','pops2_hkldt','pops2_hkldm','pops2_hkb','pops2_hks','pops2_hkstale'});
dfs.plot('pops2_hkstd','label','STD','vars',{'POPS2_STD'});
dfs.plot('pops2_hkldt','label','LD Temp','vars',{'POPS2_LDTemp'});
dfs.plot('pops2_hkldm','label','LD Mon','vars',{'POPS2_LD_Mon'});
dfs.plot('pops2_hkb','label','Baseline','vars',{'POPS2_Baseline'});
dfs.plot('pops2_hks','label','Srvr','vars',{'POPS2_Srvr'});
dfs.plot('pops2_hkstale','label','Stale','vars',{'POPS2_Stale','POPS2_DrvStale'});
dfs.end_col;
dfs.set_connection('127.0.0.1', 17006);
if nargout > 0
  dfs_out = dfs;
else
  dfs.resize(context_level);
end
