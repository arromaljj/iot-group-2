 



const measureTime = (func) => {
    const a = Date.now()
    func()
    const b = Date.now()
    return b-a

}




 const transmit = () => {

 }



const timeTaken = measureTime(() => {
    transmit()
})