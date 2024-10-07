activity_dict = {}

while(True):
    command: str = input("Enter command: ")
    if(command.upper() == "EXIT"):
        break
    elif(command.upper() == "LOAD"):
        temp_activity: str = input("Enter activity name: ")
        temp_time: int = int(input("Enter activity time: "))

        if temp_activity in activity_dict:
            activity_dict[temp_activity] += temp_time
        else:
            activity_dict[temp_activity] = temp_time
        print()

    elif(command.upper() == "SHOW ALL ACTIVITIES"):
        for activity, time in activity_dict.items():
            print(f"{activity}: {time}")
        print()

    elif(command.upper() == "SHOW TOP ACTIVITIES"):
        activities_with_time = [(activity, time) for activity, time in activity_dict.items()]
        def get_time(activity_tuple):
            return activity_tuple[1]

        activities_with_time.sort(key=get_time, reverse=True)

        for activity, time in activities_with_time[:3]:
            print(f"{activity}: {time}" + "\n")

    else:
        print("Invalid command")
        print("Use either EXIT, LOAD, SHOW ALL ACTIVITIES or SHOW TOP ACTIVITIES commands")