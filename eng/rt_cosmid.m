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
dfs.plot('tmct','label','Core T','vars',{'coretemp'});
dfs.plot('pops', 'label', 'POPS', 'plots', {'popsd','popspn','popsnum_cc','popss','popsv','popsp','popsf','popsr','popst','popspopsb','popspops2_bins'});
dfs.plot('popsd','label','Drift','vars',{'POPS_Tdrift','POPS2_Tdrift'});
dfs.plot('popspn','label','Part Num','vars',{'POPS_Part_Num','POPS2_Part_Num'});
dfs.plot('popsnum_cc','label','num cc','vars',{'POPS_num_cc','POPS2_num_cc'});
dfs.plot('popss','label','Surface','vars',{'POPS_Surf_Area','POPS2_Surf_Area'});
dfs.plot('popsv','label','Volume','vars',{'POPS_Vol_Density','POPS2_Vol_Density'});
dfs.plot('popsp','label','P','vars',{'POPS_P_mbar','POPS2_P_mbar'});
dfs.plot('popsf','label','Flow','vars',{'POPS_ccm','POPS2_ccm'});
dfs.plot('popsr','label','Raw ccs','vars',{'POPS_Flow','POPS2_Flow'});
dfs.plot('popst','label','T','vars',{'POPS_AirTemp','POPS_LDTemp','POPS2_AirTemp','POPS2_LDTemp'});
dfs.plot('popspopsb','label','POPS Bins','vars',{'POPS_Bin'});
dfs.plot('popspops2_bins','label','POPS2 Bins','vars',{'POPS2_Bin'});
dfs.end_col;
dfs.start_col;
dfs.plot('popshk', 'label', 'POPS HK', 'plots', {'popshkstd','popshkldm','popshkb','popshks','popshkstale'});
dfs.plot('popshkstd','label','STD','vars',{'POPS_STD','POPS2_STD'});
dfs.plot('popshkldm','label','LD Mon','vars',{'POPS_LD_Mon','POPS2_LD_Mon'});
dfs.plot('popshkb','label','Baseline','vars',{'POPS_Baseline','POPS2_Baseline'});
dfs.plot('popshks','label','Srvr','vars',{'POPS_Srvr','POPS2_Srvr'});
dfs.plot('popshkstale','label','Stale','vars',{'POPS_Stale','POPS_DrvStale','POPS2_Stale','POPS2_DrvStale'});
dfs.end_col;
dfs.set_connection('127.0.0.1', 17006);
if nargout > 0
  dfs_out = dfs;
else
  dfs.resize(context_level);
end
