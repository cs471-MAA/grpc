from datetime import time
import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import os

DATA_DIR = "../container_files/P50_Wmid4_tmid100_smid0_tfro30_sfro10_N2500/"
SYNC_CLIENT_FN = "clientSync.csv"
SYNC_MSGSERV_FN = "messageServiceSync.csv"
SYNC_SANITSERV_FN = "sanitServiceSync.csv"
SYNC_MOCKDATA_FN =  "mockDatabaseSync.csv"
ASYNC_CLIENT_FN = "clientAsync.csv"
ASYNC_MSGSERV_FN = "messageServiceAsync.csv"
ASYNC_SANITSERV_FN = "sanitServiceAsync.csv"
ASYNC_MOCKDATA_FN =  "mockDatabaseAsync.csv"
DF_NAMES = ["query_uid", "epoch_time"]
DF_TYPES = {DF_NAMES[0]:"Uint64", DF_NAMES[1]:"Uint64"}
CLIENT_UID_SHIFT = 28
QUERY_INDEX_MASK = 0xFFFFFFF

################### STILL IN CONSTRUCTION, used for tests and design ###################

def find_element(x, i):
    return np.nan if x.shape[0] <= i else x.values[i]

def treat_data(filepath, time_start=None, time_factor=1000.0, verbose=False):
    df = pd.read_csv(filepath, names=DF_NAMES, dtype=DF_TYPES)
    return_time_start = False
    if time_start is None:
        return_time_start = True
        time_start = df["epoch_time"].min()
    
    df["epoch_time"] -= time_start
    df["epoch_time"] /= time_factor
    df["client_uid"] = np.right_shift(df["query_uid"], CLIENT_UID_SHIFT)
    df["query_index"] = df["query_uid"] & QUERY_INDEX_MASK
    df = df.sort_values(["query_uid", "epoch_time"])
    # df = df.groupby(["query_uid", "client_uid", "query_index"]).agg(['min', 'max'])
    df = df.groupby(["query_uid", "client_uid", "query_index"]).agg([lambda x: find_element(x, 0),
                                                                     lambda x: find_element(x, 1)])
    df.columns = df.columns.set_levels(['start','end'], level=1)
    df.columns = df.columns.get_level_values(1)
    df = df.reset_index()

    # print([df["start"] > df["end"]])
    if return_time_start:
        return df, time_start   
    else:
        return df

def get_actors_df(dirpath, async_data=True, time_factor=1000.0):
    
    if async_data:
        client_df, time_start = treat_data(dirpath + ASYNC_CLIENT_FN, time_factor=time_factor)
        msgserv_df = treat_data(dirpath + ASYNC_MSGSERV_FN, time_start, time_factor=time_factor)
        sanitserv_df = treat_data(dirpath + ASYNC_SANITSERV_FN, time_start, time_factor=time_factor)
        mockdata_df = treat_data(dirpath + ASYNC_MOCKDATA_FN, time_start, time_factor=time_factor)
    else:
        client_df, time_start = treat_data(dirpath + SYNC_CLIENT_FN, time_factor=time_factor)
        msgserv_df = treat_data(dirpath + SYNC_MSGSERV_FN, time_start, time_factor=time_factor)
        sanitserv_df = treat_data(dirpath +SYNC_SANITSERV_FN, time_start, time_factor=time_factor)
        mockdata_df = treat_data(dirpath + SYNC_MOCKDATA_FN, time_start, time_factor=time_factor)
    
    return client_df, msgserv_df, sanitserv_df, mockdata_df
    
def get_data(dirpath, async_data=True, time_factor=1000.0, as_dict=True, verbose=False):
    client_df, msgserv_df, sanitserv_df, mockdata_df = get_actors_df(dirpath, async_data, time_factor)

    all = client_df.merge(msgserv_df, how='outer', on=["query_uid", "client_uid", "query_index"], suffixes=("_client", "_msg"))
    all = all.merge(sanitserv_df, how='outer', on=["query_uid", "client_uid", "query_index"], suffixes=("", "_sanit"))
    all = all.merge(mockdata_df, how='outer', on=["query_uid", "client_uid", "query_index"], suffixes=("_sanit", "_mock"))

    all = all[["client_uid", "query_index", 
            "start_client", "start_msg", "start_sanit", "start_mock",
            "end_mock", "end_sanit", "end_msg", "end_client"]]
    all["latency"] = all["end_client"] - all["start_client"]
    all = all.groupby("client_uid")
    
    if as_dict:
        return {client_uid: df.drop("client_uid", axis=1) for client_uid, df in all}
    else: 
        return all

def plot_trace(X1, X2, Y, color, label, w, ax):
    U = X2 - X1
    V = Y - Y
    ax.quiver(X1, Y, U, V,
            color=color, label=label, scale_units="xy", scale=1,
            units="y", width=w,
            # remove arrow head
            headaxislength=0, headlength=0, headwidth=1)

def new_default_figure():
    plt.figure(figsize=(10, 10), dpi=80)

def plot_calls_cascade(df, ax=None, verbose=False):
    # if df.end_client.isnull().values.any():
    #     continue
    if ax is None:
        ax = plt.axes()
    
    # print(df)
    # plt.plot(df.start_client, df.query_index, ".", color="#145ac9",label="Client Start")
    # plt.plot(df.end_client, df.query_index, ".", color="#2dbaed",label="Client End")

    w = 0.95 #df.end_client.max() / 100

    plot_trace(df.start_client, df.end_client, df.query_index, color="#145ac9", label="Client", w=w*0.99, ax=ax)
    plot_trace(df.start_msg, df.end_msg, df.query_index, color="#139415", label="Message Service", w=w*0.75, ax=ax)
    plot_trace(df.start_sanit, df.end_sanit, df.query_index, color="#bdb21c", label="Sanitize Service", w=w*0.5, ax=ax)
    plot_trace(df.start_mock, df.end_mock, df.query_index, color="#a12810", label="Mock database", w=w*0.25, ax=ax)

    if verbose:
        print(f'{w=}')
        print(f'{df.end_client.max()=}')
        print(f'{df.start_client[0]=} | {df.end_client[0]=} || ')
        print(f'{df.start_msg[0]=} | {df.end_msg[0]=} ||')
        print(f'{df.start_sanit[0]=} | {df.end_sanit[0]=} ||')
        print(f'{df.start_mock[0]=} | {df.end_mock[0]=}')

    plt.ylabel("query ID")
    plt.xlabel("time [ms]")
    plt.xlim(-df.end_client.max()*0.01, df.end_client.max()*1.01)
    plt.legend()
    return ax
    
def plot_tail_latency(df, ax=None, percs=[90, 99, 99.9], cmap="tab10", cumulative=True, density=True, alpha=1.0):

    data = df["latency"].values
    if ax is None:
        ax = plt.axes()
    percentiles = np.nanpercentile(data, percs)
    colormap = plt.cm.get_cmap(cmap)
    _, bins, patches = ax.hist(data, bins=max(len(data)//4, 100), cumulative=cumulative, density=density)

    for patch, bin in zip(patches, bins):
        i = 0
        for p in percentiles:
            if bin > p:
                i+=1
        
        rgba = colormap(i)
        rgba = (rgba[0], rgba[1], rgba[2], alpha)
        patch.set_facecolor(rgba)

        
    legend_elements = []
    percs = percs.copy()
    percs.insert(0, 0)
    for i, p in enumerate(percs):
        legend_elements.append(mpl.patches.Patch(facecolor=colormap(i), 
                                                 label='Above ' + str(p) + "th percentile"))

    ax.legend(handles=legend_elements, loc='best')
    
    return ax

def compare(adf, sdf, plot, xlim=None):
    fig, axs = plt.subplots(2,1, figsize=(10,10))
    plot(adf, axs[0])
    axs[0].set_title("Asynchronous")
    plot(sdf, axs[1])
    axs[1].set_title("Synchronous")
    for ax in axs:
        if xlim is None:
            ax.set_xlim(left=min(adf.latency.min(), sdf.latency.min()), 
                    right=max(adf.latency.max(), sdf.latency.max()))
        else:
            ax.set_xlim(xlim)
        ax.spines['top'].set_visible(False)
        ax.spines['right'].set_visible(False)
        ax.spines['bottom'].set_visible(False)
        ax.spines['left'].set_visible(False)
    return axs

def plot_tail_latency_advanced(df, ax):
    plot_tail_latency(df, cumulative=False, alpha=0.9, ax=ax)
    plot_tail_latency(df, cumulative=True, alpha=0.25, ax=ax)

def compare_tail_latency(adf, sdf):
    return compare(adf, sdf, plot_tail_latency_advanced)

def compare_calls_cascade(adf, sdf):
    return compare(adf, sdf, plot_calls_cascade, xlim=(0, max(adf.end_client.max(), sdf.end_client.max())))

def main(data_dirpath, async_data=True, verbose=False):

    all_async = get_data(data_dirpath, async_data=True, verbose=verbose)
    all_sync = get_data(data_dirpath, async_data=False, verbose=verbose)


    compare_calls_cascade(all_async[list(all_async.keys())[0]], all_sync[list(all_sync.keys())[0]])
    compare_tail_latency(all_async[list(all_async.keys())[0]], all_sync[list(all_sync.keys())[0]])


if __name__ == "__main__":
    main(DATA_DIR, async_data=True, verbose=True)
    
    plt.show()