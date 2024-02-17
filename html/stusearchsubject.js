var classes;
window.onload = function () {
    fetch("/app/stu.get_all_subjects",
        {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            },
            body: JSON.stringify({
                ID: localStorage.getItem("id"),
                token: localStorage.getItem("token")
            }),
            mode: 'no-cors'
        }).then(Response => {
            if (Response.ok == false)
                throw new Error("Network error");
            else {
                return Response.json();
            }
        }).then(json_data => {
            classes = json_data;
            for (var it of classes.subjects) {
                var teachers = "";
                for (var teacher of it.teachers) {
                    teachers = teachers + teacher + " ";
                }
                var str = "" +
                    "<tr>" +
                    "<td>" + it.name + "</td>" +
                    "<td>" + it.ID + "</td>" +
                    "<td>" + it.classroom + "</td>" +
                    "<td>" + teachers + "</td>" +
                    "<td>" + it.start + "-" + it.end + "</td>" +
                    "<td>" + it.description + "</td>" +
                    "</tr>";
                document.getElementById("subjects_from_body").innerHTML += str;
            }
        }).catch(error => {
        });
}

function search() {
    document.getElementById("subjects_from_body").innerHTML = "";
    for (var it of classes.subjects) {
        if (it.ID == Number(document.getElementById("subject_name").value) ||
            it.name == document.getElementById("subject_name").value||
            document.getElementById("subject_name").value == "") {
            var teachers = "";
            for (var teacher of it.teachers) {
                teachers = teachers + teacher + " ";
            }
            var str = "" +
                "<tr>" +
                "<td>" + it.name + "</td>" +
                "<td>" + it.ID + "</td>" +
                "<td>" + it.classroom + "</td>" +
                "<td>" + teachers + "</td>" +
                "<td>" + it.start + "-" + it.end + "</td>" +
                "<td>" + it.description + "</td>" +
                "</tr>";
            document.getElementById("subjects_from_body").innerHTML += str;
        }
    }
}
