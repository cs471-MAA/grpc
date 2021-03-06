import sys
import pandas as pd
# PINNED VALUES
PINNED_t_BACK=8000
PINNED_s_BACK=2000
PINNED_W_BACK=16
PINNED_SEED=3

def generate_parameter_files():

    # worker_numbers = [1, 8, 32, 64, 256, 512]
    worker_numbers = [512,1024,2048]
    mean_wait_times = [0, 1000, 100000]
    std_wait_times = [0, 500, 50000]
    request_proportion = [0.1, 0.5, 0.9]

    mean_delays = [0, 4000, 100000]
    std_delays = [0, 4000, 200000]

    df_builder = []
    N = 2500
    for p in request_proportion:
        P = p
        sP = str(int(p*100))
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
                            
                            df_builder.append(create_entry(P, w, t, s, td, sd, N))
    df = pd.DataFrame(df_builder)
    print(df)
    return df

def create_entry(P, W_MID, t_MID, s_MID, t_FRO=10000, s_FRO=8000, N=2500):

    STATSDIR = f"P{int(P*100)}_Wmid{W_MID}_tmid{t_MID}_smid{s_MID}_tfro{t_FRO}_sfro{s_FRO}_N{N}"
    return {"STATSDIR": STATSDIR, "P":P, "W_MSG":W_MID,"W_SANIT":W_MID,
                "t_MSG" :t_MID, "s_MSG" :s_MID, "t_SANIT" :t_MID, "s_SANIT" :s_MID,
                "t_DELAY" :t_FRO, "s_DELAY" :s_FRO, "W_MOCK": PINNED_W_BACK,
                "t_MOCK": PINNED_t_BACK, "s_MOCK": PINNED_s_BACK, "SEED": PINNED_SEED, "N": N}
    
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
# df.to_csv("data_analysis/possible_experiments.csv", sep=",", index=False)

if len(sys.argv) > 1:
    df = pd.read_csv("data_analysis/possible_experiments.csv") 
    create_env_file(df, int(sys.argv[1]))
else:
    e = create_entry(P=0.5,
                    W_MID=4,
                    t_MID=100,
                    s_MID=0,
                    t_FRO=30,
                    s_FRO=10,
                    N=2500
                    )
    create_env_file_from_dict(e)
