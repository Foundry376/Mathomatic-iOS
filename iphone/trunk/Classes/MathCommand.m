//
//  Command.m
//  mathomatic
//
//  Created by Ben Gotow on 10/24/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "MathCommand.h"
#import "mathomatic.h"

@implementation MathCommand

@synthesize showInput;
@synthesize input;
@synthesize output;

- (void)addInput:(NSString *)item
{
    if (input == nil)
        input = [[NSMutableArray alloc] init];
    [input addObject: item];
}

- (BOOL)run
{
    NSString * result;
    char * buffer = NULL;
          
    [output release];
    output = [[NSMutableArray alloc] init];
    outputEquations = [[NSMutableArray alloc] init];
    
    if ((input != nil) && ([input count] > 0))
    {
        matho_init();
        buffer = NULL;
        
        for (NSString * line in input){
            line = [line stringByReplacingOccurrencesOfString:@"π" withString:@"pi"];
            line = [line stringByReplacingOccurrencesOfString:@"±1" withString:@"sign"];
            matho_process([line cStringUsingEncoding: NSStringEncodingConversionAllowLossy], &buffer);
        }
        if (buffer != NULL) result = [NSString stringWithCString: buffer];
    }
    
    if ((result) && ([result rangeOfCharacterFromSet:[NSCharacterSet characterSetWithCharactersInString:@"abcdfghjklmnoqrstuvABCDFGHJKLMNOQRSTUVW"]].location != NSNotFound)){
        [output addObject: result];
        return NO;
    } else {
        [output addObject: result];
    }
 
    // perform post-processing of the output
    if ([[output lastObject] rangeOfString:@"sign"].location != NSNotFound){
        matho_process("simplify sign", &buffer);
        matho_process("set precision 6", &buffer);
        do {
            matho_process("#+1", &buffer);
            matho_process("approximate", &buffer);
            if (buffer != NULL){
                result = [NSString stringWithCString: buffer];
                [output addObject: result];
            }
        } while (buffer != NULL);
    }
    
    for (int ii = 0; ii < [output count]; ii++){
        NSString * eq = [self outputCleanup: [output objectAtIndex: ii]];
        
        [output replaceObjectAtIndex:ii withObject: eq];
        [outputEquations addObject: [[[EquationView alloc] initWithEquation:eq andFrame:CGRectMake(0, 0, 320, 0) andFontSize:18] autorelease]];
    }
    return YES;
}

- (NSString*)outputCleanup:(NSString*)result
{
    result = [result stringByReplacingOccurrencesOfString:@"pi#" withString:@"π"];
    result = [result stringByReplacingOccurrencesOfString:@"e#" withString:@"e"];
    result = [result stringByReplacingOccurrencesOfString:@"i#" withString:@"i"];
    result = [result stringByReplacingOccurrencesOfString:@"sign" withString:@"±1"];
    result = [result stringByReplacingOccurrencesOfString:@" " withString:@""];
    
    if ([result isEqualToString: @"Syntaxerror."])
        result = @"Invalid expression! Please check what you entered.";
        
    return result;
}

- (NSArray*)outputEquations
{
    NSMutableArray * oe = [NSMutableArray arrayWithCapacity: 1];
    
    for (int ii = 0; ii < [outputEquations count]; ii ++){
        NSString * line = [output objectAtIndex: ii];
        if ([self output: line contains:@"="] && ([self output: line contains:@"x"] || [self output: line contains:@"y"]|| [self output: line contains:@"z"]));
            [oe addObject: [outputEquations objectAtIndex: ii]];
    }
    return oe;
}

- (BOOL)output:(NSString*)line contains:(NSString*)string
{
    if ([line rangeOfString:string].location != NSNotFound)
        return YES;
    return NO;
}

- (int)attachedHeight
{
    int height = 0;
    for (EquationView * v in outputEquations)
        height += [v equationHeight];
    return height;
}

- (void)attachToView: (UIView*)view
{
    int height = 0;
    for (EquationView * v in outputEquations){
        [v setFrame: CGRectMake(0, height, [view frame].size.width, [v equationHeight])];
        [view addSubview: v];
        
        height += [v equationHeight];
    }
}

- (void)dealloc
{
    [input release];
    [output release];
    [super dealloc];
}

@end
