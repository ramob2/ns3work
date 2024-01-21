import subprocess 



def main ():
    prop_model = 0
    distance = 1
    begin = 0
    end = 43
    try:
        while 1:
            command = "./ns3 run scratch/station_propagation_simulation_station.cc -- -prop="+str(prop_model)+" -dist="+str(distance)+" -end="+str(end)+" -begin="+str(begin)
            print(command)  
            result = subprocess.run(command, check=True, shell=True,stdout=subprocess.PIPE, text=True)
            # Comupute the Return Value
            return_code = result.returncode
            #print(result.stdout)
            if return_code:
                print ("Simulation Failed!")
                return 0
            if "Failed" in result.stdout or return_code:
                if prop_model == 4:
                    print ("I'm Done!")
                    return 1 
                prop_model += 1
                if prop_model == 1:         #Fixed RSS will be simulated seperately
                    prop_model = 2
                distance = 1
            else: 
                distance += 1
    except subprocess.CalledProcessError as e:
    # If an error is raised, print it and exit the script
        print(f"Error in calling the following procedure: {e}")

if __name__ == "__main__":
    main()


