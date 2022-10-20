use crate::element::Element;
use crate::utility::Utility;

pub struct Document {
    elements: Vec<Element>,
}

impl Document {
    pub fn load(path: &std::path::Path) -> Option<Document> {
        let file_result = std::fs::File::open(path);
        match file_result {
            Ok(file) => {
                return Some(Self::load_document(&file));
            }
            Err(error) => {
                println!("Failed to open file {}: {}", path.display(), error);
            }
        }

        None
    }

    pub fn get_element(&self, name: &str) -> Option<&Element> {
        for element in &self.elements {
            if element.name == name {
                return Some(element);
            }
        }

        None
    }

    fn load_document(file: &std::fs::File) -> Document {
        let reader = std::io::BufReader::new(file);
        let xml_reader = xml::reader::EventReader::new(reader);

        let mut document = Document {
            elements: Vec::<Element>::new(),
        };

        let mut element_stack = Vec::<Element>::new();

        for element in xml_reader {
            match element {
                Ok(xml::reader::XmlEvent::StartElement { name, attributes, .. }) => {
                    let mut element = Element::new();
                    element.name = name.local_name.clone();
                    element.attributes = Utility::map_attributes(&attributes);
                    element_stack.push(element);
                }
                Ok(xml::reader::XmlEvent::EndElement { .. }) => {
                    if let Some(element) = element_stack.pop() {
                        if element_stack.is_empty() {
                            document.elements.push(element);
                        } else {
                            if let Some(last) = element_stack.last_mut() {
                                last.elements.push(element);
                            }
                        }
                    }
                }
                Ok(xml::reader::XmlEvent::Characters(string)) => {
                    if let Some(element) = element_stack.last_mut() {
                        let mut p_element = Element::new();
                        p_element.name = String::from("p");
                        p_element.value = string;
                        element.elements.push(p_element);
                    }
                }
                Err(error) => {
                    println!("Failed to read xml document: {}", error);
                }
                _ => {}
            }
        }

        document
    }
}

impl std::fmt::Display for Document {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match writeln!(f, "") {
            Err(error) => { panic!("Error writing to formatter: {}", error); }
            _ => {}
        }

        for element in &self.elements {
            match writeln!(f, "{}", element) {
                Err(error) => { panic!("Error writing to formatter: {}", error); }
                _ => {}
            }
        }

        Ok(())
    }
}
