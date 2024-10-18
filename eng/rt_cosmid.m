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
dfs.plot('tmct','label','Core T','vars',{'coretemp1','coretemp2','coretemp3','coretemp4'});
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
dfs.plot('popsb','label','Bins','vars',{'POPS_Bin01','POPS_Bin02','POPS_Bin03','POPS_Bin04','POPS_Bin05','POPS_Bin06','POPS_Bin07','POPS_Bin08','POPS_Bin09','POPS_Bin10','POPS_Bin11','POPS_Bin12','POPS_Bin13','POPS_Bin14','POPS_Bin15','POPS_Bin16'});
dfs.end_col;
dfs.start_col;
dfs.plot('popshk', 'label', 'POPS HK', 'plots', {'popshkstd','popshkldt','popshkldm','popshkb','popshks','popshkstale'});
dfs.plot('popshkstd','label','STD','vars',{'POPS_STD'});
dfs.plot('popshkldt','label','LD Temp','vars',{'POPS_LDTemp'});
dfs.plot('popshkldm','label','LD Mon','vars',{'POPS_LD_Mon'});
dfs.plot('popshkb','label','Baseline','vars',{'POPS_Baseline'});
dfs.plot('popshks','label','Srvr','vars',{'POPS_Srvr'});
dfs.plot('popshkstale','label','Stale','vars',{'POPS_Stale','POPS_DrvStale'});
dfs.end_col;
dfs.set_connection('127.0.0.1', 17006);
if nargout > 0
  dfs_out = dfs;
else
  dfs.resize(context_level);
end
