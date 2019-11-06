const images = document.querySelectorAll(".team-img");
const button = document.getElementById("button");
console.log(images);
button.addEventListener("click", (event) =>{
    images.forEach((img) => {
        img.classList.toggle("hide");
    });
});