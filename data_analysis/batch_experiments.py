import sys
import pandas as pd
# PINNED VALUES
PINNED_t_BACK=50000
PINNED_s_BACK=10000
PINNED_SEED=3
PINNED_N=2500

def generate_parameter_files():

    # worker_numbers = [1, 8, 32, 64, 256, 512]
    worker_numbers = [512,1024,2048]
    mean_wait_times = [0, 1000, 100000]
    std_wait_times = [0, 500, 50000]
    request_proportion = [0.1, 0.5, 0.9]

    # back_end_worker_numbers = [4, 16, 64, 256]
    back_end_worker_numbers = [64]
    mean_delays = [0, 4000, 100000]
    std_delays = [0, 4000, 200000]

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
                                
                                df_builder.append(create_entry(P, W_MOCK, wb, t, s, td, sd))
    df = pd.DataFrame(df_builder)
    print(df)
    return df

def create_entry(P, W_BACK, W_MID, t_MID, s_MID, t_FRO=10000, s_FRO=8000):

    STATSDIR = f"P{int(P*100)}_Wback{W_BACK}_Wmid{W_MID}_tmid{t_MID}_smid{s_MID}_tfro{t_FRO}_sfro{s_FRO}"
    return {"STATSDIR": STATSDIR, "P":P, "W_MSG":W_MID,"W_SANIT":W_MID,
                "t_MSG" :t_MID, "s_MSG" :s_MID, "t_SANIT" :t_MID, "s_SANIT" :s_MID,
                "t_DELAY" :t_FRO, "s_DELAY" :s_FRO, "W_MOCK":W_BACK, 
                "t_MOCK": PINNED_t_BACK, "s_MOCK": PINNED_s_BACK, "SEED": PINNED_SEED, "N": PINNED_N}
    
def create_env_file(df, index):
    print(df.iloc[index])
    with open(".env.custom", "w") as f:
        for col, x in zip(df.columns, df.iloc[index]):
            f.write(f"{col}={x}\n")

def create_env_file_from_dict(d):
    print(d)
    with open(".env.custom", "w") as f:
        for col, x in d.items():
            f.write(f"{col}={x}\n")

# df = generate_parameter_files()
# df.to_csv("data_analysis/experiments.csv", sep=",", index=False)

if len(sys.argv) > 1:
    df = pd.read_csv("data_analysis/experiments.csv") 
    create_env_file(df, int(sys.argv[1]))
else:
    e = create_entry(P=0.5,
                 W_BACK=64,
                 W_MID=16,
                 t_MID=10000,
                 s_MID=10000,
                t_FRO=100, 
                s_FRO=100
                 )
    create_env_file_from_dict(e)



            