import sys
import pandas as pd

def generate_parameter_files():

    # worker_numbers = [1, 8, 32, 64, 256, 512]
    worker_numbers = [512,1024,2048]
    mean_wait_times = [0, 1000, 100000]
    std_wait_times = [0, 500, 50000]
    request_proportion = [0.1, 0.5, 0.9]

    # back_end_worker_numbers = [4, 16, 64, 256]
    back_end_worker_numbers = [64]
    mean_delays = [0, 4000]
    std_delays = [0, 4000]
    # mean_delays = [1000]
    # std_delays = [1000]

    t_MOCK=50000
    s_MOCK=10000
    SEED=3
    N=2500

    df_builder = []
    
    for p in request_proportion:
        P = p
        sP = str(int(p*100))
        for wb in back_end_worker_numbers:
            W_MOCK = wb
            for w in worker_numbers:
                W_MSG = w
                W_SANIT = w
                for t in mean_wait_times:
                    t_MSG=t
                    t_SANIT=t
                    for s in std_wait_times:
                        s_SANIT=s
                        s_MSG=s        
                        for td in mean_delays:
                            t_DELAY = td
                            for sd in std_delays:
                                s_DELAY = sd
                                
                                STATSDIR = f"P{sP}_Wback{wb}_Wmid{w}_tmid{t}_smid{s}_tfro{td}_sfro{sd}"
                                df_builder.append({"STATSDIR": STATSDIR, "P":P, "W_MOCK":wb, "W_MSG":w,"W_SANIT":w,
                                                   "t_MSG" :t, "s_MSG" :s, "t_SANIT" :t, "s_SANIT" :s,
                                                   "t_DELAY" :td, "s_DELAY" :sd, "t_MOCK": t_MOCK, "s_MOCK": s_MOCK,
                                                   "SEED": SEED, "N": N})
    df = pd.DataFrame(df_builder)
    print(df)
    return df

def create_env_file(df, index):
    print(df.iloc[index])
    with open(".env.custom", "w") as f:
        for col, x in zip(df.columns, df.iloc[index]):
            f.write(f"{col}={x}\n")
    
# df = generate_parameter_files()
# df.to_csv("data_analysis/experiments.csv", sep=",", index=False)

df = pd.read_csv("data_analysis/experiments.csv") 
if len(sys.argv) > 1:
    create_env_file(df, int(sys.argv[1]))




            